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

#include <iosfwd>
#include "SliceFwd.h"

namespace lessdb {

// A Comparator object provides a total order across slices that are used as
// keys in an sstable or a database. A Comparator implementation must be
// thread-safe since lessdb may invoke its methods concurrently from multiple
// threads.
// Comparator is an option that can be customized by users.(@see Options.h)
class Comparator {
  // copyable
 public:
  virtual ~Comparator() = default;

  // The name of the comparator.
  virtual const char *Name() const = 0;

  // Three-way comparison.
  virtual int Compare(const Slice &lhs, const Slice &rhs) const = 0;

  // Advanced functions: these are used to reduce the space requirements
  // for internal data structures like index blocks.

  // If *start < limit, changes *start to a short string in [start,limit).
  // Simple comparator implementations may return with *start unchanged,
  // i.e., an implementation of this method that does nothing is correct.
  // @see SSTableBuilder::Add
  virtual void FindShortestSeparator(std::string *start,
                                     const Slice &limit) const = 0;

 protected:
  Comparator() = default;
};

// builtin Comparator, which uses lexicographic byte-wise ordering.
extern const Comparator *NewBytewiseComparator();

}  // namespace lessdb