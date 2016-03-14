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

#include "MemTable.h"
#include "InternalKey.h"
#include "Coding.h"
#include "Comparator.h"

namespace lessdb {

// TODO: do optimization.
void MemTable::Add(SequenceNumber sequence, ValueType type,
                   const Slice &key, const Slice &value) {
  // Format of an entry in MemTable
  // entry := key value
  // key   := varstring of InternalKeyBuf
  // value := varstring of value

  std::string buf;
  coding::AppendVarString(&buf, InternalKeyBuf(key, sequence, type).Data());
  coding::AppendVarString(&buf, value);

  char *entry = (char *) arena_.allocate(buf.length());
  memcpy(entry, buf.data(), buf.length());
  table_.Insert(entry);
}

static inline Slice GetVarString(const char *s) {
  Slice tmp(s, 5);
  uint32_t len;
  coding::GetVar32(&tmp, &len);
  return Slice(tmp.RawData(), len);
}

MemTable::MemTable(const InternalKeyComparator *comparator) :
    table_(&arena_,
           [comparator](const char *a_buf, const char *b_buf) -> int {
             // InternalKeys are encoded as varstrings.
             Slice a = GetVarString(a_buf);
             Slice b = GetVarString(b_buf);
             return comparator->Compare(a, b);
           }) {
}

void MemTable::Iterator::update() {
  if (iter_.Valid()) {
    e_.first = GetVarString(*iter_);

    const char *value_buf = e_.first.RawData() + e_.first.Len();
    e_.second = GetVarString(value_buf);
  }
}

MemTable::Iterator MemTable::find(const Slice &key) {
  std::string s;
  coding::AppendVarString(&s, key);
  return MemTable::Iterator(table_.Find(s.data()));
}

MemTable::Iterator MemTable::begin() const {
  return MemTable::Iterator(table_.Begin());
}

MemTable::Iterator MemTable::end() const {
  return MemTable::Iterator(table_.End());
}

} // namespace lessdb