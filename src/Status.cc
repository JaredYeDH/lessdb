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

#include "Status.h"

namespace lessdb {

std::string Status::ToString() const {
  if (!info_) return "OK";
  std::string ret;
  switch (code()) {
    case kCorruption:
      ret = "Corruption";
      break;
    case kIOError:
      ret = "IOError";
      break;
    default:
      ret = "Unknown ErrorCode";
  }
  return ret + ": " + info_->msg;
}

void Status::copy(const Status &rhs) {
  if (!rhs.info_) {
    info_.release();
  } else if (!info_) {
    info_ = std::unique_ptr<ErrorInfo>(new ErrorInfo(rhs.code(), rhs.info_->msg));
  } else {
    info_->code = rhs.info_->code;
    info_->msg = rhs.info_->msg;
  }
}

} // namespace lessdb
