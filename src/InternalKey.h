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
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
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
#include "Disallowcopying.h"

namespace lessdb {

typedef uint64_t SequenceNumber;

static constexpr SequenceNumber kMaxSequenceNumber = ((1ull << 56) - 1);

// TODO: extract ValueType out of InternalKey
enum ValueType {
  kTypeDeletion = 0x00,
  kTypeValue = 0x01
};

class InternalKey {

 public:

  // sequence and type are packed into an uint64_t integer, and placed
  // in little-endian internally.
  InternalKey(Slice user_key, SequenceNumber sequence, ValueType type);

 private:
  // [== UserKey (bytes) ==][== SequenceKey (7 bytes) ==][== ValueType (1 byte) ==]
  std::string bytes_;
};

struct InternalKeyComparator {
  bool operator()(const InternalKey &x, const InternalKey &y) const {
    // remains unimplemented
  }
};

class InternalKeyParser {
  __DISALLOW_COPYING__(InternalKeyParser);
 public:

  InternalKeyParser() = default;

  bool Parse(const InternalKey &internal_key);

  // (Debug)
  std::string Dump() const;

 private:
  Slice user_key_;
  SequenceNumber sequence_;
  ValueType type_;
};

} // namespace lessdb