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

#include <boost/crc.hpp>

#include "LogWriter.h"
#include "Files.h"
#include "DataView.h"

namespace lessdb {
namespace log {

// WriteRecord splits a user record into multiple fragments.
Status Writer::WriteRecord(const Slice &record) {
  Status s;
  size_t left = record.Len();
  const char *p = record.RawData();
  bool begin = true;

  // for each iteration we deal with one fragment
  do {
    assert(block_offset_ <= kBlockSize);

    size_t avail = kBlockSize - block_offset_;
    if (avail < kHeaderSize) {
      // trailer, consists entirely zero bytes.
      s = file_->Append(Slice("\0\0\0\0\0\0", avail));
      if (!s)
        return s;
      block_offset_ = 0;
    }

    assert(kBlockSize - block_offset_ >= kHeaderSize);
    avail = kBlockSize - block_offset_ - kHeaderSize;

    RecordType type;

    if (begin && avail >= left) {
      type = RecordType::kFull;
    } else if (begin) {
      type = RecordType::kFirst;
    } else if (avail >= left) {
      type = RecordType::kLast;
    } else {
      type = RecordType::kMiddle;
    }

    size_t fragment_length = (left < avail) ? left : avail;
    s = writeFragment(p, fragment_length, type);
    if (!s)
      return s;

    left -= fragment_length;
    p += fragment_length;
    block_offset_ += fragment_length;

    begin = false;
  } while (left > 0);

  assert(s);
  return s;
}

//
// record :=
//    checksum: uint32	    // crc32c of type and data[] ; little-endian
//    length:   uint16		// little-endian
//    type:     uint8		// One of FULL, FIRST, MIDDLE, LAST
//    data:     uint8[length]
//
Status Writer::writeFragment(const char *fragment, size_t l, RecordType type) {
  Status s;
  char buf[7];
  boost::crc_32_type crc;

  crc.process_bytes(fragment, l);
  DataView(buf).WriteNum(static_cast<uint32_t>(crc.checksum()));
  DataView(buf + 4).WriteNum(static_cast<uint16_t>(l));
  DataView(buf + 6).WriteNum(static_cast<uint8_t>(type));

  s = file_->Append(Slice(buf, kHeaderSize));
  if (!s)
    return s;

  s = file_->Append(Slice(fragment, l));
  if (!s)
    return s;

  s = file_->Flush();
  return s;
}

}  // namespace log
}  // namespace lessdb
