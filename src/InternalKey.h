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

#include "Slice.h"
#include "DBFormat.h"
#include "Disallowcopying.h"
#include "Comparator.h"

namespace lessdb {

class InternalKeyBuf {
 public:
  InternalKeyBuf(Slice user_key, SequenceNumber sequence, ValueType type);

  Slice Data() const {
    return bytes_;
  }

 private:
  // bytes_     := UserKey SequenceKey ValueType
  // UserKey    := byte*
  // Sequence   := 7bytes
  // ValueType  := uint8
  std::string bytes_;
};

struct InternalKey {
  // (Debug)
  // std::string Dump() const;

  explicit InternalKey(const Slice &key);

  Slice user_key;
  SequenceNumber sequence;
  ValueType type;
};

class InternalKeyComparator {
 public:
  //
  // InternalKeyComparator uses user_comparator to compares the
  // user_keys of two InternalKey objects.
  //
  explicit InternalKeyComparator(const Comparator *user_comparator)
      : comparator_(user_comparator) {}

  InternalKeyComparator(const InternalKeyComparator &) = default;

  int Compare(const InternalKey &lhs, const InternalKey &rhs) const;

  int Compare(const Slice &lhs, const Slice &rhs) const {
    return Compare(InternalKey(lhs), InternalKey(rhs));
  }

 private:
  const Comparator *comparator_;
};

}  // namespace lessdb