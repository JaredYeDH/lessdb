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

#include <cstddef>

#include "Disallowcopying.h"
#include "IteratorFacade.h"
#include "Options.h"
#include "Status.h"

namespace boost {
template <class T> class intrusive_ptr;
}  // namespace boost

namespace lessdb {

class Options;
class RandomAccessFile;
class Block;
class BlockConstIterator;
class SSTable;
class TwoLevelIterator;

using TwoLevelIteratorFacade =
    IteratorFacadeNoValueType<TwoLevelIterator, ForwardIteratorTag, true>;

class TwoLevelIterator : public TwoLevelIteratorFacade {
  // intentionally copyable
  friend class SSTable;
  friend class IteratorCoreAccess;

 public:
  TwoLevelIterator(TwoLevelIterator&&) = default;

  ~TwoLevelIterator();

  TwoLevelIterator& operator=(TwoLevelIterator&& rhs) = default;

  Slice Key() const;

  Slice Value() const;

 private:
  TwoLevelIterator(BlockConstIterator* data_iter,
                   BlockConstIterator* index_iter, const SSTable* table);

  TwoLevelIterator() = default;

  void increment();

  // void decrement();

  bool equal(const TwoLevelIterator& other) const;

  bool valid() const;

 private:
  std::unique_ptr<BlockConstIterator> data_iter_;
  std::unique_ptr<BlockConstIterator> index_iter_;
  const Block* block_;
  const SSTable* table_;
};

// SSTable, short for Sorted String Table, is an on-disk storage format
// contains a sorted list of key-value string pairs. Once written, an
// SSTable cannot be modified or updated, i.e., an SSTable is immutable
// and persistent.
// @see http://leveldb.googlecode.com/svn/trunk/doc/table_format.txt for the
// format of SSTable.
class SSTable {
  __DISALLOW_COPYING__(SSTable);

 public:
  // Attempt to open the table that is stored in bytes[0..file_size) of "file",
  // and read the metadata entries necessary to allow retrieving data from the
  // table.
  // If successful, sets "s" ok and returns the newly opened table. If there was
  // an error while initializing the table, sets "s" a non-ok status and returns
  // NULL.
  //
  // The client should delete the returned SSTable when no longer needed.
  // *file must remain live while this SSTable is in use.
  static SSTable* Open(const Options& options, RandomAccessFile* file,
                       uint64_t file_size, Status& s);

  friend class TwoLevelIterator;
  typedef TwoLevelIterator ConstIterator;

  // @MayGenerateErrorStatus.
  ConstIterator begin() const;

  ConstIterator end() const;

  // Searches the record with specified key in data blocks.
  // @MayGenerateErrorStatus.
  ConstIterator find(const Slice& key) const;

  // @MayGenerateErrorStatus.
  boost::intrusive_ptr<Block> ObtainBlockByIndexIterator(
      const BlockConstIterator& it) const;

  SSTable() = default;

 private:
  // Rather than holding the entire bunch of data blocks, an SSTable only keeps
  // the index block.
  RandomAccessFile* file_;
  std::unique_ptr<Block> index_block_;
  Options options_;

  mutable Status stat_;
};

}  // namespace lessdb