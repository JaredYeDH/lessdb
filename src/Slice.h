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

#include <string>
#include <cassert>

namespace lessdb {

// A Slice object wraps a "const char *" or a "const std::string&" but
// without copying their contents.
//
// NOTE: A Slice doesn't hold the ownership of the string.
//
class Slice {

 public:

  Slice(const std::string &s) :
      Slice(s.data(), s.size()) {
  }

  Slice(const char *s) :
      Slice(s, strlen(s)) {
  }

  Slice(const char *s, size_t n) :
      str_(s),
      len_(n) {
  }

  constexpr Slice(std::nullptr_t p = nullptr) :
      str_(nullptr),
      len_(0) {
  }

  char operator[](size_t n) const { return str_[n]; }

  size_t Len() const { return len_; }

  const char *RawData() const { return str_; }

  bool Empty() const { return len_ == 0; }

  int Compare(const Slice &rhs) const {
    return memcmp(str_, rhs.RawData(), len_);
  }

  void Skip(size_t n) {
    assert(n <= len_);
    str_ += n;
    len_ -= n;
  }

  void CopyTo(char *dest, bool appendEndingNull = true) const {
    memcpy(dest, str_, len_);
    if (appendEndingNull) {
      dest[len_] = '\0';
    }
  }

 private:
  const char *str_;
  size_t len_;
};

inline bool operator==(const Slice &lhs, const Slice &rhs) {
  return lhs.Compare(rhs) == 0;
}

inline std::ostream &operator<<(std::ostream &stream, const Slice &s) {
  return stream << s.RawData();
}

inline Slice operator+(const Slice &lhs, const Slice &rhs) {
  return Slice(lhs.RawData(), lhs.Len() + rhs.Len());
}

} // namespace lessdb