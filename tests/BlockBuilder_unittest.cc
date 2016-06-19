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
#include "Comparator.h"

using namespace lessdb;
using namespace test;

TEST(Basic, Empty) {
  Options options;
  options.block_restart_interval = 0;
  BlockBuilder builder(&options);
  BlockContent content;
  content.data = builder.Finish();
  Block block(content, options.comparator);

  ASSERT_TRUE(block.begin() == block.end());
}

TEST(Basic, Build) {
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

    for (const auto& it : table) {
      builder.Add(it.first, it.second);
    }

    BlockContent content;
    content.data = builder.Finish();

    Block block(content, options.comparator);

    auto it = block.begin();
    for (auto it2 = table.begin(); it2 != table.end(); it2++, it++) {
      assert(block.end() != it);
      ASSERT_EQ(it.Key().ToString(), it2->first);
      ASSERT_EQ(it.Value().ToString(), it2->second);
    }
    assert(block.end() == it);

    for (auto it2 = table.rbegin(); it2 != table.rend(); it2++) {
      if (RandomIn(0, 1) == 0) {
        ASSERT_TRUE(block.find(it2->first) != block.end());
        ASSERT_EQ(block.find(it2->first).Key().ToString(), it2->first);
        ASSERT_EQ(block.find(it2->first).Value().ToString(), it2->second);

        ASSERT_EQ(block.lower_bound(it2->first).Key().ToString(), it2->first);
        ASSERT_EQ(block.lower_bound(it2->first).Value().ToString(), it2->second);
      }
    }
  }
}