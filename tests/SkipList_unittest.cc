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
#include <thread>
#include <condition_variable>
#include <boost/thread/latch.hpp>
#include <boost/thread/thread.hpp>

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
  ASSERT_EQ(*(++it), 4);

  it = l.LowerBound(2);
  ASSERT_EQ(*it, 2);

  it = l.UpperBound(3);
  ASSERT_EQ(*it, 4);

  ASSERT_EQ(++it, l.End());

  it = l.LowerBound(5);
  ASSERT_EQ(it, l.End());
}

void verifyEqual(const std::set<int> &s,
                 const SkipList<int> &l) {
  auto it1 = l.Begin();
  auto it2 = s.begin();
  for (; it1 != l.End(); it1++, it2++) {
    ASSERT_EQ(*it1, *it2);
  }

  for (it2 = s.begin(); it2 != s.end(); it2++) {
    ASSERT_EQ(*it2, *l.LowerBound(*it2));
  }
}

TEST(Random, InsertAndLookUp) {
  SkipList<int> l;
  std::set<int> s;
  std::srand(static_cast<unsigned >(std::time(0)));

  const int N = 10000;
  for (int i = 0; i < N; i++) {
    int key = std::rand() % 4000;
    l.Insert(key);
    s.insert(key);
  }

  verifyEqual(s, l);
}

void randomAdding(int ndata, SkipList<int> *l, std::set<int> *s) {
  std::mutex mtx;
  for (int i = 0; i < ndata; i++) {
    int val = rand() % 5000;

    std::unique_lock<std::mutex> lock(mtx);
    l->Insert(val);
    lock.unlock();

    s->insert(val);
  }
}

void testConcurrentAdd(size_t nthreads) {
  SkipList<int> l;
  std::vector<std::set<int> > s(nthreads); // verifier
  boost::thread_group group;

  try {
    for (int i = 0; i < nthreads; i++) {
      group.create_thread(std::bind(randomAdding, 100, &l, &s[i]));
    }
    group.join_all();
  } catch (...) {
    EXPECT_TRUE(false);
    group.interrupt_all();
    group.join_all();
  }

  std::set<int> all;
  for (int i = 0; i < nthreads; i++) {
    all.insert(s[i].begin(), s[i].end());
  }

  verifyEqual(all, l);
}

TEST(Concurrent, Add) {
  // multi-writer
  testConcurrentAdd(10);
  testConcurrentAdd(20);
  testConcurrentAdd(50);
}

void randomAccess(boost::latch *latch, SkipList<int> *l, std::set<int> *s) {
  latch->count_down_and_wait();

  int val = std::rand() % 50;
  auto it1 = l->LowerBound(val);
  auto it2 = s->lower_bound(val);
  if (it1 == l->End()) {
    ASSERT_EQ(it2, s->end());
  } else {
    ASSERT_EQ(*it1, *it2);
  }
}

void testConcurrentAccess(size_t nthreads, size_t ndata) {

  SkipList<int> l;
  std::set<int> s;
  boost::thread_group group;

  // count-down-latch to wait until all threads are ready
  boost::latch latch(nthreads + 1);

  try {

    for (int i = 0; i < nthreads; i++) {
      group.create_thread(std::bind(randomAccess, &latch, &l, &s));
    }

    latch.count_down_and_wait();

    for (int k = 0; k < ndata; k++) {
      int val = std::rand() % 50;
      l.Insert(val);
      s.insert(val);
    }

    group.join_all();

  } catch (...) {
    EXPECT_TRUE(false);
    group.interrupt_all();
    group.join_all();
  }
}

TEST(Concurrent, Access) {
  // single-writer-multi-reader
  testConcurrentAccess(2, 10000);
  testConcurrentAccess(10, 10000);
  testConcurrentAccess(20, 10000);
  testConcurrentAccess(50, 10000);
}