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
#include <boost/smart_ptr/intrusive_ref_counter.hpp>

#include "IteratorFacade.h"
#include "Status.h"
#include "Disallowcopying.h"

namespace lessdb {

class BlockContent;
class Comparator;
class Block;

class BlockConstIterator;
using BlockConstIteratorFacade =
    IteratorFacadeNoValueType<BlockConstIterator, ForwardIteratorTag, true>;

class BlockConstIterator : public BlockConstIteratorFacade {
  friend class IteratorCoreAccess;
  friend class Block;

 public:
  Slice Key() const;

  Slice Value() const;

  Status Stat() const {
    return stat_;
  }

  const Block* GetBlock() const {
    return block_;
  }

 private:
  void increment();

  // void decrement();

  bool equal(const BlockConstIterator& other) const;

  // @param restart: index of the nearest restart point before p.
  BlockConstIterator(const char* p, const Block* block, uint32_t restart);

  void init(const char* p);

 private:
  const char* buf_;
  size_t buf_len_;
  const char* last_key_;
  size_t last_key_len_;  // is it necessary?
  uint32_t shared_;
  uint32_t unshared_;
  uint32_t value_len_;
  const Block* block_;
  uint32_t restart_pos_;
  Status stat_;  // TODO: Store errors into stat_.

  mutable std::string key_;
};

using BlockRefCounterMixin =
    boost::intrusive_ref_counter<Block, boost::thread_unsafe_counter>;

/// Block represents a block in SSTable, it provides read-only operation
/// (declaring Block without const specifier is fine).
class Block : public BlockRefCounterMixin {
  __DISALLOW_COPYING__(Block);

 public:
  Block(const BlockContent& content, const Comparator* comp);

  // empty block
  Block() = default;

  // Block data is allocated by new[].
  // @see ReadBlockFromFile in BlockUtils.h
  ~Block() {
    if (!data_)
      delete[] data_;
  }

  friend class BlockConstIterator;
  typedef BlockConstIterator ConstIterator;

  ConstIterator find(const Slice& key) const;

  ConstIterator begin() const;

  // @returns a iterator to the past-the-end element.
  ConstIterator end() const;

  // Returns an iterator pointing to the first element in the container which is
  // not considered to go before val.
  ConstIterator lower_bound(const Slice& key) const;

  const char* RawData() const {
    return data_;
  }

 private:
  uint32_t restartPoint(int id) const;

  Slice keyAtRestartPoint(int id) const;

 private:
  const char* const data_;
  const char* data_end_;  // points at the first byte of the trailer
  const Comparator* comp_;
  size_t size_;
  uint32_t num_restart_;
};

}  // namespace lessdb