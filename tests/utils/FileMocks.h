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

#include <string>

#include "Comparator.h"
#include "FileUtils.h"
#include "Status.h"

namespace test {

using namespace lessdb;

class StringSource final : public RandomAccessFile {
 public:
  explicit StringSource(const std::string &content) : content_(content) {}

  Status Read(size_t n, uint64_t offset, char *dst, Slice *result) override {
    assert(offset < content_.length());

    n = (offset + n < content_.length() ? n : content_.length() - offset);
    memcpy(dst, content_.data() + offset, n);
    (*result) = Slice(dst, n);

    return Status::OK();
  }

 private:
  std::string content_;
};

class StringSink final : public WritableFile {
 public:
  StringSink() : closed_(false) {}

  Status Flush() override {
    assert(!closed_);
    return Status::OK();
  }

  Status Close() override {
    assert(!closed_);
    closed_ = true;
    return Status::OK();
  }

  Status Sync() override {
    assert(!closed_);
    return Status::OK();
  }

  Status Append(const Slice &data) override {
    assert(!closed_);
    content_.append(data.RawData(), data.Len());
    return Status::OK();
  }

  const std::string &Content() const {
    return content_;
  }

 private:
  std::string content_;
  bool closed_;
};

// An STL comparator that uses a Comparator
struct STLLessThan {
  const Comparator *cmp;
  STLLessThan() : cmp(NewBytewiseComparator()) {}
  bool operator()(const std::string &a, const std::string &b) const {
    return cmp->Compare(Slice(a), Slice(b)) < 0;
  }
};

typedef std::map<std::string, std::string, STLLessThan> KVMap;

}  // namespace test