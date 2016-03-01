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

#include "Slice.h"
#include "DBFormat.h"
#include "Disallowcopying.h"

namespace lessdb {

class Comparator;

class InternalKeyBuf {

 public:

  InternalKeyBuf(Slice user_key,
                 SequenceNumber sequence,
                 ValueType type);

  Slice Data() const { return bytes_; }

 private:
  // [== UserKey (bytes) ==][== SequenceKey (7 bytes) ==][== ValueType (1 byte) ==]
  std::string bytes_;
};

// InternalKeyComparator uses user_comparator to compares the
// user_keys of two InternalKey objects.
extern const Comparator *
    InternalKeyComparator(const Comparator *user_comparator);

struct InternalKey {
  // (Debug)
  // std::string Dump() const;

  InternalKey(const Slice &key);

  Slice user_key;
  SequenceNumber sequence;
  ValueType type;
};

} // namespace lessdb