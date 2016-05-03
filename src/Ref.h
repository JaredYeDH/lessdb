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

#include <cassert>

/**
 * Ref is used for reference count management. If a class inherits from Ref,
 * then it is easy to be shared in different places.
 */
class Ref {
 public:
  virtual ~Ref() = default;

  /**
   * Retains the ownership.
   *
   * This increases the Ref's reference count.
   */
  void Retain() {
    assert(referenceCount_ > 0 && "reference count should be greater than 0");
    ++referenceCount_;
  }

  /**
   * Releases the ownership immediately.
   *
   * This decrements the Ref's reference count.
   *
   * If the reference count reaches 0 after the decrement, this Ref is
   * destructed.
   */
  void Release() {
    assert(referenceCount_ > 0 && "reference count should be greater than 0");
    --referenceCount_;
    if (referenceCount_ == 0) {
      delete this;
    }
  }

  /**
   * Returns the Ref's current reference count.
   */
  size_t ReferenceCount() const {
    return referenceCount_;
  }

 protected:
  /**
   * The Ref's reference count is 1 after construction.
   */
  Ref() : referenceCount_(1) {}

 protected:
  /// count of references
  size_t referenceCount_;
};