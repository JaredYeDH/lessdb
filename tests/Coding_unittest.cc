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

#include "Coding.h"
#include "Slice.h"

using namespace lessdb;
using namespace lessdb::coding;

// Test that encoding routines generate little-endian encodings
TEST(Coding, EncodingOutput) {
  std::string dst;
  AppendFixed32(&dst, 0x04030201);
  ASSERT_EQ(4, dst.size());
  ASSERT_EQ(0x01, static_cast<int>(dst[0]));
  ASSERT_EQ(0x02, static_cast<int>(dst[1]));
  ASSERT_EQ(0x03, static_cast<int>(dst[2]));
  ASSERT_EQ(0x04, static_cast<int>(dst[3]));

  dst.clear();
  AppendFixed64(&dst, 0x0807060504030201ull);
  ASSERT_EQ(8, dst.size());
  ASSERT_EQ(0x01, static_cast<int>(dst[0]));
  ASSERT_EQ(0x02, static_cast<int>(dst[1]));
  ASSERT_EQ(0x03, static_cast<int>(dst[2]));
  ASSERT_EQ(0x04, static_cast<int>(dst[3]));
  ASSERT_EQ(0x05, static_cast<int>(dst[4]));
  ASSERT_EQ(0x06, static_cast<int>(dst[5]));
  ASSERT_EQ(0x07, static_cast<int>(dst[6]));
  ASSERT_EQ(0x08, static_cast<int>(dst[7]));
}

TEST(Coding, Varint32) {
  std::string s;
  for (uint32_t i = 0; i < (32 * 32); i++) {
    uint32_t v = (i / 32) << (i % 32);
    AppendVar32(&s, v);
  }

  Slice p(s);
  for (uint32_t i = 0; i < (32 * 32); i++) {
    ASSERT_TRUE(p.Len() >= 0);
    uint32_t expected = (i / 32) << (i % 32);
    uint32_t actual;
    try {
      GetVar32(&p, &actual);
    } catch (std::exception &e) {
      fprintf(stderr, "%s\n", e.what());
      ASSERT_TRUE(false);
    }
    ASSERT_EQ(expected, actual);
  }
  ASSERT_EQ(p.RawData(), s.data() + s.size());
}

TEST(Coding, Varint64) {
  // Construct the list of values to check
  std::vector<uint64_t> values;
  // Some special values
  values.push_back(0);
  values.push_back(100);
  values.push_back(~static_cast<uint64_t>(0));
  values.push_back(~static_cast<uint64_t>(0) - 1);
  for (uint32_t k = 0; k < 64; k++) {
    // Test values near powers of two
    const uint64_t power = 1ull << k;
    values.push_back(power);
    values.push_back(power - 1);
    values.push_back(power + 1);
  }

  std::string s;
  for (size_t i = 0; i < values.size(); i++) {
    AppendVar64(&s, values[i]);
  }

  Slice p(s);
  for (size_t i = 0; i < values.size(); i++) {
    uint64_t actual;
    try {
      GetVar64(&p, &actual);
    } catch (std::exception &e) {
      fprintf(stderr, "%s\n", e.what());
      ASSERT_TRUE(false);
    }
    ASSERT_EQ(values[i], actual);
  }
  ASSERT_EQ(p.RawData(), s.data() + s.size());
}


TEST(Coding, Strings) {
  std::string s;

  try {
    AppendVarString(&s, Slice(""));
    AppendVarString(&s, Slice("foo"));
    AppendVarString(&s, Slice("bar"));
    AppendVarString(&s, Slice(std::string(200, 'x')));
  } catch (std::exception &e) {
    fprintf(stderr, "%s\n", e.what());
    ASSERT_TRUE(false);
  }

  Slice input(s);
  Slice v;

  GetVarString(&input, &v);
  ASSERT_EQ("", v.ToString());

  GetVarString(&input, &v);
  ASSERT_EQ("foo", v.ToString());

  GetVarString(&input, &v);
  ASSERT_EQ("bar", v.ToString());

  GetVarString(&input, &v);
  ASSERT_EQ(std::string(200, 'x'), v.ToString());

  ASSERT_EQ("", input.ToString());
}

