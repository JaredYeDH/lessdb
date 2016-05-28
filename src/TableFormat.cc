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
 * all
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

#include <string>
#include <cassert>

#include "TableFormat.h"
#include "Coding.h"
#include "DataView.h"
#include "Status.h"

namespace lessdb {

std::string BlockHandle::EncodeToString() const {
  std::string r;
  coding::AppendVar64(&r, offset);
  coding::AppendVar64(&r, size);
  return r;
}

Status BlockHandle::DecodeFrom(Slice *s, BlockHandle *handle) {
  try {
    coding::GetVar64(s, &handle->offset);
    coding::GetVar64(s, &handle->size);
  } catch (std::exception &e) {
    return Status::Corruption(e.what());
  }
  return Status::OK();
}

std::string Footer::EncodeToString() const {
  std::string r(index_handle.EncodeToString() +
                mataindex_handle.EncodeToString());
  assert(r.length() < 40 - 8);
  r.reserve(40);
  DataView(&r[31]).WriteNum(kTableMagicNumber);
  return r;
}

Status Footer::DecodeFrom(Slice *buf, Footer *footer) {
  Status s = BlockHandle::DecodeFrom(buf, &footer->index_handle);
  if (s) {
    s = BlockHandle::DecodeFrom(buf, &footer->mataindex_handle);
  }
  return s;
}

}  // namespace lessdb
