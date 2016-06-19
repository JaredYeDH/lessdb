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

#include <glog/logging.h>
#include <gtest/gtest.h>

#include "SSTableBuilder.h"
#include "TestUtils.h"
#include "SSTable.h"
#include "Block.h"
// Must include Block.h or compiler will warn that Block is an incomplete type.

using namespace lessdb;
using namespace test;

const Block* SSTable::TEST_GetIndexBlock() const {
  assert(index_block_);
  return index_block_.get();
}

Block* SSTableBuilder::TEST_GetIndexBlock() {
  BlockContent content;
  content.data = index_block_.Finish();
  return new Block(content, options_->comparator);
}

TEST(Build, Empty) {
  Options options;
  StringSink sink;

  SSTableBuilder builder(&options, &sink);
  builder.Finish();

  /// Empty table includes:
  //  An emtpy data block +
  //  Index block +
  //  Footer

  ASSERT_EQ(builder.NumEntries(), 0);
  ASSERT_TRUE(sink.Content().size() > Footer::kEncodedLength);

  StringSource source(sink.Content());
  Status s;
  std::unique_ptr<SSTable> table(
      SSTable::Open(options, &source, sink.Content().size(), s));
  ASSERT_TRUE(s) << s.ToString();

  const Block* blk(table->TEST_GetIndexBlock());
  int num_entries = 0;
  for (auto it = blk->begin(); it != blk->end(); it++) {
    num_entries++;

    Slice hbuf = it.Value();
    BlockHandle handle;
    s = BlockHandle::DecodeFrom(&hbuf, &handle);
    ASSERT_TRUE(s.IsOK()) << s.ToString();
  }
  ASSERT_EQ(num_entries, 1);

  // Empty SSTable is a special case that's uninteresting to lessdb.
  ASSERT_TRUE(table->begin() != table->end());
}

TEST(Basic, Random) {
  Options options;

  for (int num_entries = 1; num_entries < 2000;
       num_entries += (num_entries < 50) ? 1 : 200) {
    KVMap table;
    StringSink sink;
    SSTableBuilder builder(&options, &sink);

    fprintf(stderr, "num_entries = %d\n", num_entries);

    for (int i = 0; i < num_entries; ++i) {
      auto key = RandomString(RandomIn(0, 1 << 4));
      auto value = RandomString(RandomIn(0, 1 << 5));
      table.emplace(std::make_pair(std::move(key), std::move(value)));
    }

    for (const auto& it : table) {
      builder.Add(it.first, it.second);
    }

    builder.Finish();
    StringSource source(sink.Content());
    Status s;
    std::unique_ptr<SSTable> sst(
        SSTable::Open(options, &source, sink.Content().size(), s));

    auto it = sst->begin();
    for (auto it2 = table.begin(); it2 != table.end(); it2++, it++) {
      assert(sst->end() != it);
      ASSERT_EQ(it.Key().ToString(), it2->first);
      ASSERT_EQ(it.Value().ToString(), it2->second);
    }

    ASSERT_TRUE(sst->end() == it);
  }
}