/**
 * Copyright (C) 2016, Wu Tao. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <boost/any.hpp>

#include "SSTable.h"
#include "FileUtils.h"
#include "TableFormat.h"
#include "BlockUtils.h"
#include "CacheStrategy.h"
#include "Block.h"

namespace lessdb {

SSTable *SSTable::Open(const Options &options, RandomAccessFile *file,
                       uint64_t file_size, Status &s) {
  // Read the footer of the file, obtain a block handle for index block. Read
  // the index block identified by the handle.
  std::unique_ptr<SSTable> table;

  Slice footer_content;
  char footer_buf[Footer::kEncodedLength];
  s = file->Read(Footer::kEncodedLength, file_size - Footer::kEncodedLength,
                 footer_buf, &footer_content);
  if (!s)
    return nullptr;

  if (footer_content.Len() < Footer::kEncodedLength) {
    s = Status::Corruption("SSTable::Open: Footer too small");
    return nullptr;
  }

  Footer footer;
  if (!(s = Footer::DecodeFrom(&footer_content, &footer)))
    return nullptr;

  table.reset(new SSTable());

  ReadOptions read_options;
  table->index_block_.reset(ReadBlockFromFile(
      file, read_options, options.comparator, footer.index_handle, s));
  if (!s)
    return nullptr;

  table->file_ = file;
  return table.release();
}

SSTable::ConstIterator SSTable::begin() const {
  auto block = ObtainBlockByIndexIterator(index_block_->begin());
  if (!block) {
    return end();
  }
  return TwoLevelIterator(new BlockConstIterator(block->begin()),
                          new BlockConstIterator(index_block_->begin()), this);
}

SSTable::ConstIterator SSTable::end() const {
  return TwoLevelIterator();
}

SSTable::ConstIterator SSTable::find(const Slice &key) const {
  auto idx_it = index_block_->lower_bound(key);
  if (idx_it == index_block_->end()) {
    // index < key
    fprintf(stderr, "this 1\n");
    fprintf(stderr, "%s\n", key.RawData());
    for(auto it = index_block_->begin(); it!=index_block_->end(); it++) {
      fprintf(stderr, "[%s,%s] ", it.Key().RawData(), it.Value().RawData());
    }
    fprintf(stderr, "\n");
    return end();
  }
  // index >= key
  auto block = ObtainBlockByIndexIterator(idx_it);
  if (!block) {
    fprintf(stderr, "this 2\n");
    return end();
  }
  auto blck_it = block->find(key);
  if (blck_it == block->end()) {
    fprintf(stderr, "this 3\n");
    return end();
  }
  return TwoLevelIterator(new BlockConstIterator(blck_it),
                          new BlockConstIterator(idx_it), this);
}

Block *SSTable::ObtainBlockByIndexIterator(const BlockConstIterator &it) const {
  // Obtain a block handle that contains index of the data block.
  BlockHandle handle;
  Slice block_index_buf = it.Value();
  stat_ = BlockHandle::DecodeFrom(&block_index_buf, &handle);
  if (!stat_) {
    return nullptr;
  }

  boost::intrusive_ptr<Block> block;
  CacheStrategy *cache = options_.block_cache;

  if (cache) {
    // The key of a BlockCache is in format of:
    // key          := unique_id block_offset
    // unique_id    := uint64
    // block_offset := uint64
    char key_buf[16];
    (*reinterpret_cast<uint64_t *>(key_buf)) = cache->NewId();
    (*reinterpret_cast<uint64_t *>(key_buf + 8)) = handle.offset;
    Slice key(key_buf, sizeof(key_buf));

    CacheStrategy::HANDLE h = cache->Lookup(key);
    if (h != NULL) {
      //
      block = *boost::unsafe_any_cast<decltype(block)>(&cache->Value(h));
    } else {
      cache->Insert(key, block);
    }
  }

  if (!block) {
    /// Iff cache is not set or block is not found in cache.
    ReadOptions read_options;
    block.reset(ReadBlockFromFile(file_, read_options, options_.comparator,
                                  handle, stat_));
    if (!stat_)
      return nullptr;
  }
  return block.detach();
}

TwoLevelIterator::TwoLevelIterator(BlockConstIterator *data_it,
                                   BlockConstIterator *idx_it,
                                   const SSTable *table)
    : data_iter_(data_it),
      index_iter_(idx_it),
      block_(data_it->GetBlock()),
      table_(table) {}

TwoLevelIterator::TwoLevelIterator(const TwoLevelIterator &rhs) {
  if (rhs.valid()) {
    data_iter_.reset(new BlockConstIterator(*rhs.data_iter_));
    index_iter_.reset(new BlockConstIterator(*rhs.index_iter_));
    block_ = rhs.block_;
    table_ = rhs.table_;
  }
}

TwoLevelIterator &TwoLevelIterator::operator=(const TwoLevelIterator &rhs) {
  TwoLevelIterator tmp(rhs);
  std::swap(*this, tmp);
  return *this;
}

Slice TwoLevelIterator::Key() const {
  assert(valid());
  return data_iter_->Key();
}

Slice TwoLevelIterator::Value() const {
  assert(valid());
  return data_iter_->Value();
}

bool TwoLevelIterator::equal(const TwoLevelIterator &other) const {
  if (!data_iter_ || !other.data_iter_) {  // end() == end()
    return data_iter_.get() == other.data_iter_.get();
  }
  return *data_iter_ == *other.data_iter_;
}

void TwoLevelIterator::increment() {
  assert(valid());

  (*data_iter_)++;
  if ((*data_iter_) == data_iter_->GetBlock()->end()) {
    (*index_iter_)++;
    if ((*index_iter_) == index_iter_->GetBlock()->end()) {
      // Iff the iterator hits the end, swap it with the end() iterator.
      TwoLevelIterator tmp;
      std::swap(*this, tmp);
    } else {
      auto block = table_->ObtainBlockByIndexIterator(*index_iter_);
      TwoLevelIterator tmp(new BlockConstIterator(block->begin()),
                           new BlockConstIterator(*index_iter_), table_);
      std::swap(*this, tmp);
    }
  }
}

// @return false iff the data iterator hits the end.
inline bool TwoLevelIterator::valid() const {
  return *this != TwoLevelIterator();
}

}  // namespace lessdb