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

#include <gtest/gtest.h>
#include <boost/any.hpp>

#include "CacheStrategy.h"
#include "Slice.h"

using namespace lessdb;

TEST(Correctness, T1) {
  CacheStrategy::HANDLE look;
  int val;

  std::unique_ptr<CacheStrategy> lru_strategy(CacheStrategy::Default(2));
  lru_strategy->Insert("1", 1);
  lru_strategy->Insert("2", 1);

  // look up an entry that already exists
  look = lru_strategy->Lookup("1");
  val = boost::any_cast<int>(lru_strategy->Value(look));
  ASSERT_EQ(val, 1);

  // look up an entry that are discarded
  lru_strategy->Insert("3", 2);
  look = lru_strategy->Lookup("2");
  ASSERT_TRUE(look == NULL);

  look = lru_strategy->Lookup("3");
  val = boost::any_cast<int>(lru_strategy->Value(look));
  ASSERT_EQ(val, 2);

  // look up an entry that are erased.
  lru_strategy->Erase("1");
  look = lru_strategy->Lookup("1");
  ASSERT_TRUE(look == NULL);

  // update the value of an existing entry
  look = lru_strategy->Insert("3", 3);
  val = boost::any_cast<int>(lru_strategy->Value(look));
  ASSERT_EQ(val, 3);
}
