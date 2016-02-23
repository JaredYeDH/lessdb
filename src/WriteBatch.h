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

#include <memory>

#include "Disallowcopying.h"

namespace lessdb {

class Slice;
class Status;
class WriteBatchImpl;

/**
 * WriteBatch holds a collection of updates.
 */
class WriteBatch {
  __DISALLOW_COPYING__(WriteBatch);

 public:

  WriteBatch();

  // Store the element (key, value) into database.
  void Put(const Slice &key, const Slice &value);

  // Erase the specific update if exists.
  // Internally, a "tombstone" record is appended for deletes.
  void Delete(const Slice &key);

  // WriteBatch::Handler provides interfaces to iterate the
  // contents of WriteBatch, and for each of the updates, do
  // the corresponding operation.
  class Handler {
   public:
    virtual ~Handler() = default;
    virtual void Put(const Slice &key, const Slice &value) = 0;
    virtual void Delete(const Slice &key) = 0;
  };
  // Possible error status:
  // Status::Corruptions
  Status Iterate(Handler *handler) const;

 private:
  std::unique_ptr<WriteBatchImpl> pImpl_;
};

} // namespace lessdb