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

#include <mutex>

#include "Coding.h"
#include "InternalKey.h"
#include "DataView.h"
#include "Comparator.h"

namespace lessdb {

/// InternalKeyBuf

static inline uint64_t packSequenceAndType(SequenceNumber sequence,
                                           ValueType type) {
  return static_cast<uint64_t>((sequence << 8) | static_cast<uint8_t>(type));
}

InternalKeyBuf::InternalKeyBuf(Slice key, SequenceNumber seq, ValueType type) {
  bytes_.append(key.RawData(), key.Len());
  coding::AppendFixed64(&bytes_, packSequenceAndType(seq, type));
}

/// InternalKeyComparator

int InternalKeyComparator::Compare(const InternalKey &lhs,
                                   const InternalKey &rhs) const {
  int r = comparator_->Compare(lhs.user_key, rhs.user_key);
  if (r == 0) {
    uint64_t l_num = packSequenceAndType(lhs.sequence, lhs.type);
    uint64_t r_num = packSequenceAndType(rhs.sequence, rhs.type);
    if (l_num < r_num)
      r = +1;
    else if (l_num > r_num)
      r = -1;
  }
  return r;
}

/// InternalKey

InternalKey::InternalKey(const Slice &key) {
  assert(key.Len() >= 8);
  user_key = Slice(key.RawData(), key.Len() - 8);

  uint64_t packed =
      ConstDataView(key.RawData() + key.Len() - 8).ReadNum<uint64_t>();
  sequence = (packed >> 8);
  type = static_cast<ValueType>(packed & ((1 << 8) - 1));
}

}  // namespace lessdb
