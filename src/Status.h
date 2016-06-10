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

#include "Slice.h"

namespace lessdb {

class Status {
 private:
  enum ErrorCodes { kOK = 0, kCorruption = 1, kIOError = 2 };

 public:
  // An empty Status will be treated as an OK status.
  Status() noexcept = default;

  // copyable
  Status(const Status &rhs) noexcept {
    copy(rhs);
  }

  Status &operator=(const Status &rhs) noexcept {
    copy(rhs);
    return (*this);
  }

  Status &operator=(Status &&) = default;

  // Return a success state.
  // The cost of creating an success state is much cheaper than an error state.
  // The error information is generated only when the Status object is not an
  // OK object, which contains merely a pointer.
  static Status OK() {
    return Status();
  }

  bool IsOK() const {
    return code() == ErrorCodes::kOK;
  }

  explicit operator bool() const {
    return IsOK();
  }

  // Data corruption.
  static Status Corruption(const Slice &msg) {
    return Status(kCorruption, msg);
  }

  bool IsCorruption() const {
    return code() == kCorruption;
  }

  static Status IOError(const Slice &msg) {
    return Status(kIOError, msg);
  }

  bool IsIOError() const {
    return code() == kIOError;
  }

  std::string ToString() const;

  Status &operator<<(const std::string &str) {
    assert(info_);  // operator<< must not be applied to an OK Status!
    info_->msg.append(str);
    return *this;
  }

  Status &operator<<(const Slice &v) {
    return (*this) << v.ToString();
  }

  Status &operator<<(const char *v) {
    return (*this) << std::string(v);
  }

  template <class T> Status &operator<<(T v) {
    return (*this) << std::to_string(v);
  }

 private:
  Status(ErrorCodes errorCode, const Slice &msg) noexcept
      : info_(new ErrorInfo(errorCode, msg)) {}

  struct ErrorInfo {
    unsigned char code;
    std::string msg;

    ErrorInfo(ErrorCodes c, const Slice &s)
        : code(static_cast<unsigned char>(c)), msg(s.RawData(), s.Len()) {}
  };

  ErrorCodes code() const {
    if (!info_)
      return ErrorCodes::kOK;
    return static_cast<ErrorCodes>(info_->code);
  }

  void copy(const Status &);

 private:
  std::unique_ptr<ErrorInfo> info_;
};

}  // namespace lessdb