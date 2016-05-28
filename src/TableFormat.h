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

#include "Slice.h"

namespace lessdb {

// The file contains internal pointers.  Each such pointer is called
// a BlockHandle and contains the following information:
//    offset:	    varint64
//    size:		    varint64 /// Note that trailer of the block is not
//                           /// included in size.
// See https://developers.google.com/protocol-buffers/docs/encoding#varints
// for an explanation of varint64 format.

class Status;

struct BlockHandle {
  uint64_t offset;
  uint64_t size;

  BlockHandle() : offset(0), size(0) {}

  std::string EncodeToString() const;

  static Status DecodeFrom(Slice *s, BlockHandle *handle);

  // Maximum encoding length of a BlockHandle: 2 * sizeof(varint64)
  enum { kMaxEncodedLength = 10 + 10 };
};

struct BlockContent {
  Slice data;
};

// kTableMagicNumber was picked by running
//    echo http://code.google.com/p/leveldb/ | sha1sum
// and taking the leading 64 bits.
static const uint64_t kTableMagicNumber = 0xdb4775248b80fb57ull;

// Each block is followed by a trailer in the format of:
//     compression_type: uint8
//     crc:              uint32
// @see TableBuilder::writeBlock
static const uint64_t kBlockTrailerSize = 5;

// Footer encapsulates the fixed information stored at the tail
// end of every table file.
// The information contains the BlockHandle of the metaindex and index blocks as
// well as a magic number.
//
// metaindex_handle: char[p];      // Block handle for metaindex
// index_handle:     char[q];      // Block handle for index
// padding:          char[40-p-q]; // zeroed bytes to make fixed length
//                                    (40==2*BlockHandle::kMaxEncodedLength)
// magic:            fixed64;      // == 0xdb4775248b80fb57 (little-endian)

struct Footer {
  BlockHandle mataindex_handle;  // Block handle for metaindex
  BlockHandle index_handle;      // Block handle for index

  std::string EncodeToString() const;

  static Status DecodeFrom(Slice *s, Footer *footer);

  // Encoded length of a Footer.  Note that the serialization of a
  // Footer will always occupy exactly this many bytes.  It consists
  // of two block handles and a magic number.
  enum { kEncodedLength = 2 * BlockHandle::kMaxEncodedLength + 8 };
};

}  // namespace lessdb