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

#include <gtest/gtest.h>

#include "Slice.h"

using namespace lessdb;

TEST(Basic, Literal) {
  Slice s("abc");
  ASSERT_EQ(s.Empty(), false);
  ASSERT_EQ(s.Len(), 3);
  ASSERT_EQ(s[3], '\0');
}

TEST(Basic, StdString) {
  std::string str("abc");
  Slice s(str);
  ASSERT_EQ(s.Len(), 3);
  ASSERT_EQ(s[3], '\0');
  ASSERT_EQ(*(s.RawData() + 3), '\0');
}

TEST(Basic, Empty) {
  Slice s("");
  ASSERT_EQ(s.Empty(), true);
  ASSERT_EQ(s.Len(), 0);
  ASSERT_NE(s.RawData(), nullptr);
  ASSERT_EQ(s[0], '\0');
  ASSERT_EQ(*s.RawData(), '\0');
}

TEST(Basic, Null) {
  Slice s(nullptr);
  ASSERT_EQ(s.Empty(), true);
  ASSERT_EQ(s.Len(), 0);
  ASSERT_EQ(s.RawData(), nullptr);
}

TEST(Basic, Copy) {
  Slice s("abc");

  char actual[4];
  s.CopyTo(actual);
  ASSERT_TRUE(memcmp(actual, "abc", sizeof(actual)) == 0);
}