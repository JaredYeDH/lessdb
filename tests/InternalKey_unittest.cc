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

#include "InternalKey.h"
#include "Comparator.h"

using namespace lessdb;

TEST(InternalKeyComparator, Init) {
  ASSERT_EQ(ByteWiseComparator()->Name(), "lessdb.ByteWiseComparator");
  const Comparator *comparator = InternalKeyComparator(ByteWiseComparator());
  ASSERT_EQ(comparator->Name(), "lessdb.InternalKeyComparator");
}

TEST(InternalKeyComparator, Compare) {
  auto comparator = InternalKeyComparator(ByteWiseComparator());

  {
    InternalKeyBuf ibuf1("abc", 1, kTypeValue);
    InternalKeyBuf ibuf2("abc", 1, kTypeValue);

    ASSERT_TRUE(comparator->Compare(ibuf1.Data(), ibuf2.Data()) == 0);
  }

  {
    InternalKeyBuf ibuf1("abc", 1, kTypeValue);
    InternalKeyBuf ibuf2("abc", 2, kTypeValue);
    InternalKey ikey1(ibuf1.Data());
    InternalKey ikey2(ibuf2.Data());

    ASSERT_TRUE(ByteWiseComparator()->Compare(ikey1.user_key, ikey2.user_key) == 0);
    ASSERT_TRUE(comparator->Compare(ibuf1.Data(), ibuf2.Data()) > 0);
    ASSERT_TRUE(comparator->Compare(ibuf2.Data(), ibuf1.Data()) < 0);
  }
}

TEST(InternalKey, Init) {
  InternalKeyBuf ibuf("", 0, kTypeValue);
  InternalKey ikey(ibuf.Data());
  ASSERT_EQ(ikey.user_key, "");
  ASSERT_EQ(ikey.sequence, 0);
  ASSERT_EQ(ikey.type, kTypeValue);

  ibuf = InternalKeyBuf("abc", 10, kTypeDeletion);
  ikey = InternalKey(ibuf.Data());
  ASSERT_EQ(ikey.user_key, "abc");
  ASSERT_EQ(ikey.sequence, 10);
  ASSERT_EQ(ikey.type, kTypeDeletion);
}
