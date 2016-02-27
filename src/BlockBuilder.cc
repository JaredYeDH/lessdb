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

#include "BlockBuilder.h"
#include "Options.h"
#include "DataView.h"
#include "Coding.h"

namespace lessdb {

BlockBuilder::BlockBuilder(const Options *option) :
    finished_(false),
    option_(option),
    count_(0) {
  restarts_.push_back(0);
}

// Returns the length of identical prefix in k1, k2.
// Returns 0 iff no shared prefix is found.
static inline size_t
sharedPrefix(const Slice &k1, const Slice &k2) {
  size_t i = 0;
  size_t min_len = std::min(k1.Len(), k2.Len());
  while (i < min_len && k1[i] == k2[i]) i++;
  return i;
}

// An entry for a particular key-value pair has the form:
//     shared_bytes_length   := varint32
//     unshared_bytes_length := varint32
//     value_length          := varint32
//     key_delta             := char[unshared_bytes]
//     value                 := char[value_length]
// shared_bytes == 0 for restart points.
//
void BlockBuilder::Add(Slice key, Slice value) {

  size_t shared = 0;
  if (count_ < option_->block_restart_interval) {
    shared = sharedPrefix(last_key_, key);
  } else {
    count_ = 0;
    restarts_.push_back(static_cast<uint32_t >(buf_.size()));
  }

  size_t unshared = key.Len() - shared;

  // append a new entry into buffer.
  coding::AppendVar32(&buf_, static_cast<uint32_t>(shared));
  coding::AppendVar32(&buf_, static_cast<uint32_t>(unshared));
  coding::AppendVar32(&buf_, static_cast<uint32_t>(value.Len()));
  buf_.append(key.RawData() + shared, unshared); // non-shared key_delta
  buf_.append(value.RawData(), value.Len()); // value data

  // update state
  last_key_ = (key.RawData() + shared);
  count_++;
}

// The trailer of the block has the form:
//     restarts: uint32[num_restarts]
//     num_restarts: uint32
// restarts[i] contains the offset within the block of the ith restart point.
//
Slice BlockBuilder::Finish() {

  char ibuf[sizeof(uint32_t)];

  // append the trailer of the block

  // append offset of restart points
  for (int i = 0; i < restarts_.size(); ++i) {
    DataView(ibuf).WriteNum(restarts_[i]);
    buf_.append(ibuf, sizeof(uint32_t));
  }

  // append num_restarts
  DataView(ibuf).WriteNum(static_cast<uint32_t >(restarts_.size()));
  buf_.append(ibuf, sizeof(uint32_t));

  finished_ = true;
  return Slice(buf_);
}

} // namespace lessdb
