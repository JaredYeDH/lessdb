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

#include <gtest/gtest.h>
#include <unordered_map>

#include "MemTable.h"
#include "Status.h"
#include "WriteBatch.h"
#include "WriteBatchImpl.h"

using namespace lessdb;

TEST(Basic, Empty) {
  InternalKeyComparator cmp(NewBytewiseComparator());
  MemTable table(cmp);
  ASSERT_TRUE(table.begin() == table.end());
  ASSERT_TRUE(table.find("abc") == table.end());
}

TEST(Basic, Add) {
  InternalKeyComparator cmp(NewBytewiseComparator());
  MemTable table(cmp);
  table.Add(1, kTypeValue, "abc", "def");

  Slice key = InternalKeyBuf("abc", 1, kTypeValue).Data();
  ASSERT_TRUE(table.find(key) != table.end());

  auto iter = table.find(key);
  ASSERT_EQ(InternalKey(iter->first).user_key, Slice("abc"));
  ASSERT_EQ(InternalKey(iter->first).sequence, 1);
  ASSERT_EQ(InternalKey(iter->first).type, kTypeValue);
  ASSERT_EQ(iter->second, Slice("def"));
}

TEST(Basic, WriteBatchInsert) {
  InternalKeyComparator cmp(NewBytewiseComparator());
  MemTable memtable(cmp);
  WriteBatch batch;
  std::unordered_map<std::string, std::string> table;
  table["k1"] = "v1";
  table["k2"] = "v2";
  table["k3"] = "v3";
  table["k4"] = "v4";

  for (const auto& it : table) {
    batch.Put(it.first, it.second);
  }
  ASSERT_TRUE(batch.InsertInto(&memtable).IsOK());

  for (auto it1 = memtable.begin(); it1 != memtable.end(); it1++) {
    std::string key(it1->first.RawData(), it1->first.Len() - 8);
    ASSERT_EQ(table[key], it1->second.ToString());
  }
}