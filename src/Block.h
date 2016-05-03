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

#pragma once

#include "Disallowcopying.h"
#include "IteratorFacade.h"
#include "Slice.h"
#include "Options.h"
#include "Status.h"

namespace lessdb {

class Block {
  __DISALLOW_COPYING__(Block);

 public:
  class Iterator : public IteratorFacade<Iterator, BidirectionalIteratorTag> {};

 private:
};

// BlockHandle is used to position the block in sstable.
struct BlockHandle {
  size_t offset;  // The offset of the block in sstable
  size_t size;    // The size of the block.
};

struct BlockContents {
  Slice data;

  // Read the block identified by "handle" from "file".  On failure
  // return non-OK.  On success fill the BlockContent and return OK.
  Status ReadBlock(const BlockHandle &handle, const ReadOptions &options) {}
};

}  // namespace lessdb