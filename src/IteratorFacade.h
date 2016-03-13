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

namespace lessdb {

struct ForwardIteratorTag { };
struct BidirectionalIteratorTag: public ForwardIteratorTag { };

class IteratorCoreAccess {

  template<class D, class T, class C> friend
  class IteratorFacade;

 private:

  template<class Facade>
  static void increment(Facade &f) {
    f.increment();
  }

  template<class Facade>
  static void decrement(Facade &f) {
    f.decrement();
  }

  template<class Facade>
  static bool equal(Facade const &f1, Facade const &f2) {
    return f1.equal(f2);
  }

  template<class Facade>
  static typename Facade::reference dereference(Facade const &f) {
    return f.dereference();
  }

  // Objects of this class are useless.
  IteratorCoreAccess() = delete;
};

template<
    class Derived,
    class T,
    class Category>
class IteratorFacade {

 public:

  typedef T value_type;
  typedef T &reference;
  typedef T *pointer;
  typedef Category iterator_category;

  reference operator*() const {
    return IteratorCoreAccess::dereference(this->derived());
  }

  pointer operator->() const {
    return std::addressof(*this->derived());
  }

  Derived &operator--() {
    IteratorCoreAccess::decrement(this->derived());
    return this->derived();
  }

  Derived operator--(int) {
    Derived tmp(this->derived());
    --*this;
    return tmp;
  }

  Derived &operator++() {
    IteratorCoreAccess::increment(this->derived());
    return this->derived();
  }

  Derived operator++(int) {
    Derived tmp(this->derived());
    ++*this;
    return tmp;
  }

  friend bool operator==(const Derived &lhs, const Derived &rhs) {
    return IteratorCoreAccess::equal(lhs, rhs);
  }

 protected:

  //
  // Curiously Recurring Template interface.
  //
  Derived &derived() {
    return *static_cast<Derived *>(this);
  }

  Derived const &derived() const {
    return *static_cast<Derived const *>(this);
  }
};

} // namespace lessdb
