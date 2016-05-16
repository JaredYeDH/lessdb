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
#include "Comparator.h"
#include "Slice.h"

namespace lessdb {

class BytewiseComparator final : public Comparator {
 public:
  const char *Name() const override {
    return "lessdb.BytewiseComparator";
  }

  int Compare(const Slice &lhs, const Slice &rhs) const override {
    return lhs.Compare(rhs);
  }

  void FindShortestSeparator(std::string *start,
                             const Slice &limit) const override {
    // if start < limit, start and limit must share same prefix (the prefix can
    // be null), assume i is the smallest index where start.length < i ||
    // start[i] < limit[i]
    // then start[i] = limit[i] - 1;
    assert(Compare(*start, limit) < 0);
    size_t sep = start->length();
    for (size_t i = 0; i < start->length(); i++) {
      assert(i < limit.Len());
      if ((*start)[i] == limit[i])
        continue;
      assert((*start)[i] < limit[i]);
      (*start)[i] = static_cast<char>(limit[i] - 1);
      sep = i;
      break;
    }

    if (sep == start->length()) {
      assert(limit.Len() > sep);
      (*start)[sep] = static_cast<char>(limit[sep] - 1);
    }
    assert(Compare(*start, limit) < 0);
  }
};

static const Comparator *byteWiseComparator = nullptr;

static void InitModule() {
  byteWiseComparator = new BytewiseComparator();
}

// thread-safe singleton
const Comparator *NewBytewiseComparator() {
  static std::once_flag flag;
  std::call_once(flag, InitModule);
  return byteWiseComparator;
}

}  // namespace lessdb