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

#include <functional>
#include <iterator>
#include <random>

#include "Disallowcopying.h"

namespace lessdb {

// SkipLists are a probabilistic balanced data structure.
// This implementation is based on the paper
// [Skip Lists: A Probabilistic Alternative to Balanced Trees] by William Pugh.
//
// NOTE: Allocated nodes are never deleted until the SkipList is destroyed.

template<class T, class Compare = std::less<T> >
class SkipList {
  __DISALLOW_COPYING__(SkipList);

  struct Node;

  static const unsigned MaxLevel = 12;

 public:

  struct Iterator: public std::iterator<std::bidirectional_iterator_tag, T> {
    typedef typename Iterator::pointer pointer;
    typedef typename Iterator::value_type value_type;

    // intentionally copyable
    pointer ptr;

    // prefix
    T &operator++() {
    }
    T &operator--() {
    }

    // postfix
    T operator++(int) {
    }
    T operator--(int) {
    }

    const value_type &operator*() const {

    }

    const value_type *operator->() const {

    }

    friend bool operator==(const Iterator &x, const Iterator &y) noexcept {
    }

    friend bool operator!=(const Iterator &X, const Iterator &y) noexcept {
    }
  };

 public:

  ~SkipList() noexcept = default;

  Iterator Insert(const T &key);

  Iterator Find(const T &key);

  bool Empty() const noexcept {
    return head_ == nullptr;
  }

  Iterator LowerBound(const T &key);

  Iterator UpperBound(const T &key);

  bool Size() const noexcept;

  Iterator Begin() const noexcept;

  Iterator End() const noexcept;

 private:

  int randomLevel();

 private:
  Node *const head_;
};

template<class T, class Compare>
struct SkipList<T, Compare>::Node {

  const T key;

  explicit Node(const T &k) :
      key(k) {
  }

  Node *Next() const {

  }

  Node *Prev() const {

  }
};


template<class T, class Compare>
int SkipList<T, Compare>::randomLevel() {
  int height = 1;
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0, 3);
  while (height < MaxLevel && distribution(generator) == 0) height++;
  return height;
}

} // namespace lessdb