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

InternalKeyComparator::InternalKeyComparator(const Comparator *user_comparator)
    : Comparator(
          // Order by:
          //    increasing user key (according to user-supplied comparator)
          //    decreasing sequence number
          //    decreasing type (though sequence# should be enough to
          //    disambiguate)
          [user_comparator](const Slice &lhs, const Slice &rhs) -> int {
            InternalKey l_key(lhs), r_key(rhs);
            int r = user_comparator->Compare(l_key.user_key, r_key.user_key);
            if (r == 0) {
              uint64_t l_num = packSequenceAndType(l_key.sequence, l_key.type);
              uint64_t r_num = packSequenceAndType(r_key.sequence, r_key.type);
              if (l_num < r_num)
                r = +1;
              else if (l_num > r_num)
                r = -1;
            }
            return r;
          },
          "lessdb.InternalKeyComparator") {}

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
