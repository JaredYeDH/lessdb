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

#include "Status.h"

using namespace lessdb;

TEST(Basic, OK) {
  Status s;
  ASSERT_TRUE(s.IsOK());

  s = Status::OK();
  ASSERT_EQ(s.IsOK(), true);
  ASSERT_EQ(sizeof(s), sizeof(std::unique_ptr<int>));
  ASSERT_EQ(s.ToString(), "OK");
}

TEST(Basic, Corruption) {
  Status s = Status::Corruption("test");
  ASSERT_EQ(s.IsCorruption(), true);
  ASSERT_EQ(s.ToString(), "Corruption: test");
}

TEST(Basic, Copy) {
  Status s = Status::Corruption("test");
  ASSERT_EQ(s.IsOK(), false);

  Status s2 = s;
  ASSERT_EQ(s2.IsCorruption(), true);
  ASSERT_EQ(s2.ToString(), s.ToString());

  Status s3(s2);
  ASSERT_EQ(s2.IsCorruption(), true);
  ASSERT_EQ(s2.ToString(), s.ToString());
}