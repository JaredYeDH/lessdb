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
#include <random>

#include "SkipList.h"

using namespace lessdb;

TEST(Basic, Init) {
  SkipList<int> l;
  ASSERT_TRUE(l.Empty());

  ASSERT_EQ(l.Begin(), l.End());
}

TEST(Basic, Insert) {
  SkipList<int> l;
  ASSERT_EQ(*(l.Insert(3)), 3);
  ASSERT_EQ(*(l.Insert(2)), 2);
  l.Insert(4);
  l.Insert(3);

  auto it = l.Begin();
  ASSERT_EQ(*it, 2);
  it++;
  ASSERT_EQ(*it, 3);
  it++;
  ASSERT_EQ(*it, 3);
  it++;
  ASSERT_EQ(*it, 4);
  it++;
  ASSERT_EQ(it, l.End());
}

TEST(Basic, LookUp) {
  SkipList<int> l;
  l.Insert(3);
  l.Insert(2);
  l.Insert(4);
  l.Insert(3);

  auto it = l.LowerBound(3);
  ASSERT_EQ(*(++it), 3);

  it = l.LowerBound(2);
  ASSERT_EQ(*it, 2);

  it = l.UpperBound(3);
  ASSERT_EQ(*it, 4);

  ASSERT_EQ(++it, l.End());

  it = l.LowerBound(5);
  ASSERT_EQ(it, l.End());
}

TEST(Random, InsertAndLookUp) {
  SkipList<int> l;
  std::set<int> s;
  std::default_random_engine gen;
  std::uniform_int_distribution<int> distrib;

  const int N = 10000;
  for (int i = 0; i < N; i++) {
    int key = distrib(gen);
    l.Insert(key);
    s.insert(key);
  }

  auto it1 = l.Begin();
  auto it2 = s.begin();
  for (; it1 != l.End(); it1++, it2++) {
    ASSERT_EQ(*it1, *it2);
  }

  for (it2 = s.begin(); it2 != s.end(); it2++) {
    ASSERT_EQ(*it2, *l.LowerBound(*it2));
  }
}