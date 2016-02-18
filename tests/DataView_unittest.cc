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

#include "DataView.h"

using namespace lessdb;

TEST(Basic, BigEndian) {
  char buf[128];

  int64_t n1 = LLONG_MAX - 1000;
  DataView(buf, BigEndian::value).WriteNum(n1);
  int32_t n2 = INT_MAX - 1000;
  DataView(buf, BigEndian::value).WriteNum(n2, sizeof(n1));
  int16_t n3 = SHRT_MAX - 100;
  DataView(buf, BigEndian::value).WriteNum(n3, sizeof(n1) + sizeof(n2));

  ASSERT_EQ(n1, DataView(buf, BigEndian::value).ReadNum<int64_t>());
  ASSERT_EQ(n2, DataView(buf, BigEndian::value).ReadNum<int32_t>(sizeof(n1)));
  ASSERT_EQ(n3, DataView(buf, BigEndian::value).ReadNum<int16_t>(sizeof(n1) + sizeof(n2)));
}

TEST(Basic, LittleEndian) {
  char buf[128];

  uint64_t n1 = ULLONG_MAX - 1000;
  DataView(buf, LittleEndian::value).WriteNum(n1);
  uint32_t n2 = UINT_MAX - 1000;
  DataView(buf, LittleEndian::value).WriteNum(n2, sizeof(n1));
  uint16_t n3 = USHRT_MAX - 100;
  DataView(buf, LittleEndian::value).WriteNum(n3, sizeof(n1) + sizeof(n2));

  ASSERT_EQ(n1, DataView(buf, LittleEndian::value).ReadNum<uint64_t>());
  ASSERT_EQ(n2, DataView(buf, LittleEndian::value).ReadNum<uint32_t>(sizeof(n1)));
  ASSERT_EQ(n3, DataView(buf, LittleEndian::value).ReadNum<uint16_t>(sizeof(n1) + sizeof(n2)));
}


TEST(Basic, Native_Big_Little) {
  char buf[sizeof(uint32_t) * 3];
  uint32_t native = 1234;

  DataView dv1(buf, NativeEndian::value);
  dv1.WriteNum(native);

  DataView dv2(buf + sizeof(uint32_t), LittleEndian::value);
  dv2.WriteNum(native);

  DataView dv3(buf + sizeof(uint32_t) * 2, BigEndian::value);
  dv3.WriteNum(native);

  ASSERT_EQ(native, dv1.ReadNum<uint32_t>());
  ASSERT_EQ(native, dv2.ReadNum<uint32_t>());
  ASSERT_EQ(native, dv3.ReadNum<uint32_t>());
}
