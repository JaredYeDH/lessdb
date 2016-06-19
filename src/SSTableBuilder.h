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

#pragma once

#include <boost/crc.hpp>

#include "Disallowcopying.h"
#include "Status.h"
#include "Options.h"
#include "FileUtils.h"
#include "BlockBuilder.h"
#include "TableFormat.h"
#include "Comparator.h"

namespace lessdb {

class Block;
class SSTableBuilder {
  __DISALLOW_COPYING__(SSTableBuilder);

 public:
  // Create a builder that will store the contents of the table it is
  // building in *file. Does not close the file.  It is up to the
  // caller to close the file after calling Finish().
  SSTableBuilder(const Options *options, WritableFile *file)
      : data_block_(options),
        index_block_(options),
        options_(options),
        file_(file),
        pending_index_entry_(false),
        num_entries_(0) {}

  // Add key,value to the table being constructed.
  // REQUIRES: key is after any previously added key according to comparator.
  // REQUIRES: Finish(), Abandon() have not been called
  Status Add(const Slice &key, const Slice &value) {
    if (pending_index_entry_) {
      // after a flush of data block
      // append new index entry
      options_->comparator->FindShortestSeparator(&last_key_, key);
      // pending_handle_.offset now points at index block (updated by
      // writeBlock), with pending_handle_.size indicating the size of the
      // previous data_block.
      std::string index_value = pending_handle_.EncodeToString();
      index_block_.Add(last_key_, index_value);
      pending_index_entry_ = false;
    }

    num_entries_++;
    data_block_.Add(key, value);
    last_key_.assign(key.RawData(), key.Len());

    if (data_block_.Size() >= options_->block_size) {
      return flush();
    }
    return Status::OK();
  }

  Status Finish() {
    // flush the last data block
    Status s = flush();
    if (!s)
      return s;

    // recording the index information of the last data block

    if (!last_key_.empty() &&
        last_key_.back() < std::numeric_limits<char>::max())
      last_key_.back() = static_cast<char>(last_key_.back() + 1);
    else
      last_key_.push_back('a');

    index_block_.Add(last_key_, pending_handle_.EncodeToString());

    // write index block
    s = writeBlock(&index_block_);
    if (!s)
      return s;

    // write footer
    Footer footer;
    footer.index_handle = pending_handle_;
    s = file_->Append(footer.EncodeToString());
    return s;
  }

  size_t NumEntries() const {
    return num_entries_;
  }

 private:
  // Flush the building data block to file.
  // pending_index_entry_ will be updated.
  Status flush() {
    Status s = writeBlock(&data_block_);
    if (!s)
      return s;
    pending_index_entry_ = true;
    data_block_.Reset();
    return Status::OK();
  }

  // pending_handle will be updated.
  Status writeBlock(BlockBuilder *block) {
    // Each block is followed by a trailer in the format of:
    //     compression_type: uint8
    //     crc:              uint32

    Status s;

    // process block data
    Slice block_buf = block->Finish();
    s = file_->Append(block_buf);
    if (!s)
      return s;

    // process trailer
    char trailer[kBlockTrailerSize];
    boost::crc_32_type crc;
    crc.process_bytes(block_buf.RawData(), block_buf.Len());
    DataView(trailer + sizeof(uint8_t)).WriteNum(crc.checksum());
    s = file_->Append(Slice(trailer, kBlockTrailerSize));
    if (!s)
      return s;

    uint64_t block_size_with_trailer = block_buf.Len() + kBlockTrailerSize;
    pending_handle_.size = block_size_with_trailer;
    pending_handle_.offset += block_size_with_trailer;
    //    fprintf(stderr, "pending handle size: %llu, offset %llu\n",
    //            pending_handle_.size, pending_handle_.offset);

    return Status::OK();
  }

 public:
  Block *TEST_GetIndexBlock();

 private:
  // In use:
  // Options::block_size
  const Options *options_;
  BlockBuilder data_block_;
  BlockBuilder index_block_;
  WritableFile *file_;
  std::string last_key_;

  // We do not emit the index entry for a block until we have seen the
  // first key for the next data block.  This allows us to use shorter
  // keys in the index block.  For example, consider a block boundary
  // between the keys "the quick brown fox" and "the who".  We can use
  // "the r" as the key for the index block entry since it is >= all
  // entries in the first block and < all entries in subsequent
  // blocks.
  //
  // Invariant: pending_index_entry_ is true only if data_block is empty.
  bool pending_index_entry_;
  BlockHandle pending_handle_;  // Handle to add to index block

  size_t num_entries_;
};

}  // namespace lessdb