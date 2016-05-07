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
#include "SliceFwd.h"

namespace boost {
class any;
}  // namespace boost

namespace lessdb {

// LessDB allows users to specify the internal cache strategy in Options.
class CacheStrategy {
  __DISALLOW_COPYING__(CacheStrategy);

 public:
  // opaque handle pointed to an entry stored in the cache
  struct Handle {};
  typedef Handle *HANDLE;

  CacheStrategy(size_t capacity){};

  virtual ~CacheStrategy() = default;

  // Insert a mapping from key->value into the cache.
  // Else return a handle that corresponds to the mapping.
  virtual HANDLE Insert(const Slice &key, const boost::any &value) = 0;

  virtual void Erase(const Slice &key) = 0;

  // If the cache has no mapping for "key", returns NULL.
  // NOTE: the underlying data of Cache is not guaranteed to be unchanged after
  // calling of Lookup.
  virtual HANDLE Lookup(const Slice &key) = 0;

  // Return the value encapsulated in a valid handle.
  virtual boost::any Value(HANDLE handle) const = 0;

  // Default implementation of Cache uses a least-recently-used eviction policy.
  static std::unique_ptr<CacheStrategy> Default(size_t capacity);
};

}  // namespace lessdb