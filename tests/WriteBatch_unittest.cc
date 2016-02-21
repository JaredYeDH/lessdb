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

#include "WriteBatch.h"
#include "WriteBatchImpl.h"

using namespace lessdb;


class WriteBatchPrinter: public WriteBatch::Handler {

 public:

  void Put(const Slice &key, const Slice &value) override {
    state_.append("Put(");
    state_.append(key.RawData(), key.Len());
    state_.append(", ");
    state_.append(value.RawData(), value.Len());
    state_.append(")");
  }

  void Delete(const Slice &key) override {
    state_.append("Delete(");
    state_.append(key.RawData(), key.Len());
    state_.append(")");
  }

  std::string ToString() const {
    return state_;
  }

 private:
  std::string state_;
};

TEST(Basic, Init) {
  WriteBatchImpl batch;
  WriteBatchPrinter printer;

  Status stat = batch.Iterate(&printer);
  ASSERT_TRUE(stat.IsOK()) << stat.ToString();
  ASSERT_EQ(printer.ToString(), "");
  ASSERT_EQ(batch.Count(), 0);
}

TEST(Basic, MultipleElements) {
  WriteBatchImpl batch;
  WriteBatchPrinter printer;
  batch.PutRecord("foo", "bar");
  batch.DeleteRecord("box");
  batch.PutRecord("baz", "boo");

  Status stat = batch.Iterate(&printer);
  ASSERT_TRUE(stat.IsOK()) << stat.ToString();
  ASSERT_EQ(printer.ToString(), "Put(foo, bar)Delete(box)Put(baz, boo)");
  ASSERT_EQ(batch.Count(), 3);
}