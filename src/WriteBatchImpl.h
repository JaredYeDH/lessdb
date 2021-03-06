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

#include <folly/Likely.h>
#include <folly/Varint.h>

#include "Coding.h"
#include "DBFormat.h"
#include "DataView.h"
#include "Status.h"

#pragma once

namespace lessdb {

// bytes_ is the internal representation of WriteBatch.
//
// bytes_ :=
//    sequence: fixed64
//    count: fixed32
//    data: record[count]
// record :=
//    kTypeValue varstring varstring  |
//    kTypeDeletion varstring
// varstring :=
//    len: varint32
//    data: uint8[len]
//
// Keys and values are copied into the memory held by WriteBatch, in order to
// keep the lifetime of them. May these strings be held by reference count?
//
class WriteBatchImpl {
  static constexpr size_t kCountSize = sizeof(uint32_t);
  static constexpr size_t kSeqSize = sizeof(uint64_t);
  static constexpr size_t kHeaderSize = kSeqSize + kCountSize;

 public:
  WriteBatchImpl() {
    bytes_.resize(kHeaderSize);
  }

  int Count() const {
    return ConstDataView(&bytes_[kSeqSize]).ReadNum<int>();
  }

  void SetCount(int count) {
    DataView(&bytes_[kSeqSize]).WriteNum(count);
  }

  void PutRecord(const Slice &key, const Slice &value) {
    SetCount(Count() + 1);  // count++
    bytes_.push_back(static_cast<char>(kTypeValue));
    coding::AppendVarString(&bytes_, key);
    coding::AppendVarString(&bytes_, value);
  }

  void DeleteRecord(const Slice &key) {
    SetCount(Count() + 1);  // count++
    bytes_.push_back(static_cast<char>(kTypeDeletion));
    coding::AppendVarString(&bytes_, key);
  }

  Status Iterate(WriteBatch::Handler *handler) const {
    Slice s(bytes_);
    if (UNLIKELY(s.Len() < kHeaderSize)) {  // fast path
      return Status::Corruption("malformed WriteBatch (too small)");
    }
    s.Skip(kHeaderSize);

    while (!s.Empty()) {
      ValueType type = static_cast<ValueType>(s[0]);
      s.Skip(1);

      Slice key, value;

      switch (type) {
        case kTypeValue:
          try {
            coding::GetVarString(&s, &key);
            coding::GetVarString(&s, &value);
          } catch (std::invalid_argument &e) {
            return Status::Corruption("Bad WriteBatch put") << e.what();
          }
          handler->Put(key, value);
          break;
        case kTypeDeletion:
          try {
            coding::GetVarString(&s, &key);
          } catch (std::invalid_argument &e) {
            return Status::Corruption("Bad WriteBatch delete") << e.what();
          }
          handler->Delete(key);
          break;
        default:
          return Status::Corruption("Undefined ValueType");
      }
    }
    return Status::OK();
  }

 private:
  std::string bytes_;
};

}  // namespace lessdb