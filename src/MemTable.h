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

#include <mutex>

#include "DBFormat.h"
#include "Disallowcopying.h"
#include "SkipList.h"
#include "Slice.h"

namespace lessdb {

class InternalKeyComparator;

class MemTable {
  __DISALLOW_COPYING__(MemTable);

 private:
  typedef std::function<int(const char *, const char *)> Compare;
  typedef SkipList<const char *, Compare> Table;

 public:
  explicit MemTable(const InternalKeyComparator *comparator);

  void Add(SequenceNumber sequence, ValueType type, const Slice &key,
           const Slice &value);

  size_t BytesUsed() const {
    return arena_.bytesUsed();
  }

 public:
  // stdlib-like APIs.

  // Entry.first is the InternalKey.
  // Entry.second is the value.
  typedef std::pair<Slice, Slice> Entry;

  struct ConstIterator;

  ConstIterator find(const Slice &key);

  ConstIterator begin() const;

  ConstIterator end() const;

 private:
  SysArena arena_;
  Table table_;
  std::mutex mtx_;
};

struct MemTable::ConstIterator
    : public IteratorFacade<ConstIterator, Entry, ForwardIteratorTag, true> {
  // MemTable is the only caller of ConstIterator's constructor.
  friend class MemTable;
  typedef IteratorFacade<ConstIterator, Entry, ForwardIteratorTag, true> Facade;
  typedef Facade::Reference Reference;

 private:
  // Constructor of ConstIterator must be hidden from user.
  explicit ConstIterator(Table::ConstIterator iter) : iter_(iter) {
    update();
  }

  // Updates the value of e_ each time iter_ changes.
  // udpate() only makes changes on e_, so const specifier is safe here.
  void update() const;

  // The following functions are required for IteratorFacade.

  friend class IteratorCoreAccess;

  // Lazy derefence, udpate e_ only when necessary.
  Reference dereference() const {
    update();
    return e_;
  }

  void increment() {
    iter_++;
  }

  bool equal(const ConstIterator &other) const {
    return iter_ == other.iter_;
  }

 private:
  Table::ConstIterator iter_;
  mutable Entry e_;
};

}  // namespace lessdb