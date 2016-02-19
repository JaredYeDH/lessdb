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

#pragma once

#include "Disallowcopying.h"
#include "Slice.h"

namespace lessdb {

class Slice;
struct Options;

class BlockBuilder {
  __DISALLOW_COPYING__(BlockBuilder);

 public:

  BlockBuilder(const Options *option);

  void Add(Slice key, Slice value);

  // Returns a Slice that refers to the underlying block contents.
  Slice Finish();

 private:

  // In use:
  // option->block_restart_interval
  const Options *option_;

  std::string buf_; // Destination buffer.
  std::string last_key_; // The last key that's added into block.
  bool finished_; // Has Finish() been called?
  std::vector<uint32_t> restarts_; // Restart points.
  int count_; // Number of entries emitted since last restart point.
};

} // namespace lessdb