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

#include <glog/logging.h>
#include <gtest/gtest.h>

#include "SSTableBuilder.h"
#include "TestUtils.h"
#include "SSTable.h"
#include "Block.h"
#include "BlockUtils.h"
// Must include Block.h or compiler will warn that Block is an incomplete type.

using namespace lessdb;

TEST(Basic, Empty) {
  Options options;
  test::StringSink sink;

  SSTableBuilder builder(&options, &sink);
  builder.Finish();

  /// Empty table includes:
  //  An emtpy data block +
  //  Index block +
  //  Footer

  ASSERT_EQ(builder.NumEntries(), 0);
  ASSERT_TRUE(sink.Content().size() > Footer::kEncodedLength);

  test::StringSource source(sink.Content());
  Status s;

  std::unique_ptr<SSTable> table(
      SSTable::Open(options, &source, sink.Content().size(), s));
  if (!s) {
    LOG(FATAL) << s.ToString() << std::endl;
  }
  ASSERT_TRUE(table->begin() == table->end());
}