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

#include "WriteBatch.h"
#include "MemTable.h"
#include "Status.h"
#include "WriteBatchImpl.h"

namespace lessdb {

WriteBatch::WriteBatch() : pImpl_(new WriteBatchImpl()) {}

void WriteBatch::Put(const Slice &key, const Slice &value) {
  pImpl_->PutRecord(key, value);
}

void WriteBatch::Delete(const Slice &key) {
  pImpl_->DeleteRecord(key);
}

Status WriteBatch::Iterate(WriteBatch::Handler *handler) const {
  return pImpl_->Iterate(handler);
}

// used for WriteBatch::InsertInto
class MemTableInserter : public WriteBatch::Handler {
 public:
  MemTableInserter(MemTable *table) : table_(table), seq_(0) {}

  void Put(const Slice &key, const Slice &value) override {
    table_->Add(seq_++, kTypeValue, key, value);
  }

  void Delete(const Slice &key) override {
    table_->Add(seq_++, kTypeDeletion, key, Slice());
  }

 private:
  SequenceNumber seq_;
  MemTable *table_;
};

Status WriteBatch::InsertInto(MemTable *table) {
  MemTableInserter inserter(table);
  return pImpl_->Iterate(&inserter);
}

}  // namespace lessdb
