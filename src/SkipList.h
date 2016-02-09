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
#include <cassert>

#include "Disallowcopying.h"

namespace lessdb {

// SkipLists are a probabilistic balanced data structure.
// This implementation is based on the paper
// [Skip Lists: A Probabilistic Alternative to Balanced Trees] by William Pugh.
//
// NOTE: Allocated nodes are never deleted until the SkipList is destroyed.

// Compare takes two element keys as arguments and returns a bool.
// The expression comp(a,b), where comp is an object of this type
// and a and b are key values, shall return true if a is considered to go
// before b in the strict weak ordering the function defines.

template<class T, class Compare = std::less<T> >
class SkipList {
  __DISALLOW_COPYING__(SkipList);

  struct Node;

  static const unsigned MaxLevel = 12;

 public:

  struct Iterator {
   private:
    // intentionally copyable
    const Node *node_;

   public:

    Iterator() :
        node_(nullptr) {
    }

    explicit Iterator(const Node *n) :
        node_(n) {
    }

    // prefix
    Iterator &operator++() {
      node_ = node_->Next(0);
      return (*this);
    }

    // postfix
    Iterator operator++(int) {
      Iterator ret(node_);
      node_ = node_->Next(0);
      return ret;
    }

    const T &operator*() const {
      return node_->key;
    }

    const T *operator->() const {
      return &(node_->key);
    }

    friend bool operator==(const Iterator &x, const Iterator &y) noexcept {
      return x.node_ == y.node_;
    }

    friend bool operator!=(const Iterator &x, const Iterator &y) noexcept {
      return x.node_ == y.node_;
    }
  };

 public:

  explicit SkipList(const Compare &compare = Compare());

  ~SkipList() noexcept;

  Iterator Insert(const T &key);

//  Iterator Find(const T &key);

  bool Empty() const noexcept {
    return Begin() == End();
  }

  // Returns an iterator pointing to the first element
  // in the range [first,last) which does not compare less than val.
  Iterator LowerBound(const T &key) const;

  // Returns an iterator pointing to the first element
  // in the range [first,last) which compares greater than val.
  Iterator UpperBound(const T &key) const;

//  bool Size() const noexcept;

  const Iterator Begin() const noexcept {
    return Iterator(head_->Next(0));
  }

  const Iterator End() const noexcept {
    return Iterator(nullptr);
  }

 private:

  int getHeight() const;

  int randomLevel();

  int random();

  bool keyEq(const T &k1, const T &k2) const {
    return compare_(k1, k2) && !compare_(k1, k2);
  }

 private:
  Node *const head_;
  int height_;
  Compare compare_;

  // random number ranges in [0, 3]
  std::default_random_engine gen_;
  std::uniform_int_distribution<int> distrib_;
};

template<class T, class Compare>
struct SkipList<T, Compare>::Node {
  const T key;
  Node **forward;

  Node(const T &k) :
      key(k) {
  }

  Node *Next(int level) const {
    return forward[level];
  }

  void SetNext(Node *next, int level) {
    forward[level] = next;
  }

  static Node *Make(const T &key, int height) {
    Node *ret = new Node(key);
    ret->forward = new Node *[height];
    return ret;
  }
};

template<class T, class Compare>
inline int SkipList<T, Compare>::randomLevel() {
  int height = 1;
  while (height < MaxLevel && random() == 0) height++;
  return height;
}

template<class T, class Compare>
inline int SkipList<T, Compare>::random() {
  return distrib_(gen_);
}

template<class T, class Compare>
typename SkipList<T, Compare>::Iterator
SkipList<T, Compare>::Insert(const T &key) {
  Node *update[MaxLevel];
  int height = getHeight() - 1;
  Node *x = head_;

  for (int level = height; level >= 0; level--) {
    Node *next = x->Next(level);
    while (next != nullptr && compare_(next->key, key)) {
      x = next;
      next = x->Next(level);
    }
    // next == nullptr or x->key < key <= next->key
    update[level] = x;
  }

  x = x->Next(0); // x->key >= key or x == nullptr
  if (x != nullptr && keyEq(key, x->key)) {
    return Iterator(x);
  }

  int level = randomLevel();
  if (level > height_) {
    for (int i = height_; i < level; i++) {
      update[i] = head_;
    }
    height_ = level;
  }

  x = Node::Make(key, level);
  for (int i = 0; i < level; i++) {
    x->SetNext(update[i]->Next(i), i);
    update[i]->forward[i] = x;
  }

  return Iterator(x);
}

template<class T, class Compare>
inline int SkipList<T, Compare>::getHeight() const {
  return height_;
}

template<class T, class Compare>
typename SkipList<T, Compare>::Iterator
SkipList<T, Compare>::LowerBound(const T &key) const {
  Node *x = head_;
  int height = getHeight() - 1;

  for (int level = height; level >= 0; level--) {
    Node *next = x->Next(level);
    while (next != nullptr && compare_(next->key, key)) {
      x = next;
      next = x->Next(level);
    }
    // x->key < key <= next->key
    if (level == 0)
      return Iterator(next);
  }
  assert(0);
}

template<class T, class Compare>
typename SkipList<T, Compare>::Iterator
SkipList<T, Compare>::UpperBound(const T &key) const {
  Node *x = head_;
  int height = getHeight() - 1;

  for (int level = height; level >= 0; level--) {
    Node *next = x->Next(level);
    // key <= next->key ==> !(key > next->key)
    while (next != nullptr && !compare_(key, next->key)) {
      x = next;
      next = x->Next(level);
    }
    // x->key <= key < next->key
    if (level == 0)
      return Iterator(next);
  }
  assert(0);
}

template<class T, class Compare>
SkipList<T, Compare>::~SkipList() noexcept {
  
}

template<class T, class Compare>
SkipList<T, Compare>::SkipList(const Compare &compare) :
    distrib_(0, 3),
    head_(Node::Make(0, MaxLevel)),
    height_(1),
    compare_(compare) {
  for (int i = 0; i < MaxLevel; i++)
    head_->SetNext(nullptr, i);
}

} // namespace lessdb