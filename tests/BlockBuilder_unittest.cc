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
#include <map>

#include "BlockBuilder.h"
#include "Options.h"
#include "Block.h"
#include "TestUtils.h"
#include "TableFormat.h"

using namespace lessdb;

TEST(Basic, Empty) {
  Options options;
  options.block_restart_interval = 0;
  BlockBuilder builder(&options);
  BlockContent content;
  content.data = builder.Finish();
  Block block(content, options.comparator);

  ASSERT_TRUE(block.begin() == block.end());
}

typedef std::map<std::string, std::string> KVMap;

TEST(Basic, Build) {
  using namespace test;
  Options options;
  KVMap table;
  BlockBuilder builder(&options);

  for (int num_entries = 0; num_entries < 2000;
       num_entries += (num_entries < 50) ? 1 : 200) {
    table.clear();
    builder.Reset();

    fprintf(stderr, "num_entries = %d\n", num_entries);

    for (int i = 0; i < num_entries; ++i) {
      auto key = RandomString(RandomIn(0, 1 << 4));
      auto value = RandomString(RandomIn(0, 1 << 5));
      table.emplace(std::make_pair(std::move(key), std::move(value)));
    }

    for (const auto &it : table) {
      builder.Add(it.first, it.second);
    }

    BlockContent content;
    content.data = builder.Finish();

    Block block(content, options.comparator);

    auto it = block.begin();
    auto it2 = table.begin();
    for (; it2 != table.end(); it2++, it++) {
      assert(block.end() != it);
      ASSERT_EQ(it.Key().ToString(), it2->first);
      ASSERT_EQ(it.Value().ToString(), it2->second);
    }
    assert(block.end() == it);
  }
}