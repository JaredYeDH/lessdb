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

#pragma once

namespace lessdb {
namespace log {

// The log file contents are a sequence of 32KB blocks.
//
// Each block consists of a sequence of records:
// block := record* trailer?
// record :=
//    checksum: uint32	    // crc32c of type and data[] ; little-endian
//    length:   uint16		// little-endian
//    type:     uint8		// One of FULL, FIRST, MIDDLE, LAST
//    data:     uint8[length]

static constexpr int kBlockSize = 32768;

// Header is checksum (4 bytes), length (2 bytes), type (1 byte).
static constexpr int kHeaderSize = 4 + 2 + 1;

// FIRST, MIDDLE, LAST are types used for user records that have been split into
// multiple fragments (typically because of block boundaries).
enum class RecordType : uint8_t {

  // The FULL record contains the contents of an entire user record.
  kFull,

  // FIRST is the type of the first fragment of a user record
  kFirst,

  // MID is the type of all interior fragments of a user record.
  kMiddle,

  // LAST is the type of the last fragment of a user record
  kLast
};

}  // namespace log
}  // namespace lessdb