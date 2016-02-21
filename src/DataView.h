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

#include <cstddef>

#include "DisallowCopying.h"

namespace lessdb {

class Slice;

enum EndianType {
  kNativeEndian = 0,
  kLittleEndian = 1,
  kBigEndian = 2
};

struct NativeEndian {
  static const EndianType value = EndianType::kNativeEndian;
};

struct LittleEndian {
  static const EndianType value = EndianType::kLittleEndian;
};

struct BigEndian {
  static const EndianType value = EndianType::kBigEndian;
};

/**
 * The DataView view provides a low-level interface for reading and
 * writing multiple number types in an buffer irrespective of the
 * platform's endianness.
 *
 * Endianess independence is useful for portable binary files and
 * network I/O formats.
 */
class DataView {
  __DISALLOW_COPYING__(DataView);

 public:

  // A DataView object offers reading and writing operations on an
  // existing buffer.
  // Note: the capacity of the buffer must be ensured enough before
  // the read/write operation.
  //
  // The order of bytes in buf can be specified by endianType, which
  // is set default to little-endian.
  //
  DataView(char *buf, EndianType endianType = LittleEndian::value) :
      buf_(buf),
      endian_(endianType) {
  }

  DataView &WriteNum(unsigned char val, size_t offset = 0);
  DataView &WriteNum(char val, size_t offset = 0);
  DataView &WriteNum(unsigned short val, size_t offset = 0);
  DataView &WriteNum(short val, size_t offset = 0);
  DataView &WriteNum(unsigned val, size_t offset = 0);
  DataView &WriteNum(int val, size_t offset = 0);
  DataView &WriteNum(unsigned long long val, size_t offset = 0);
  DataView &WriteNum(long long val, size_t offset = 0);
  DataView &WriteNum(float val, size_t offset = 0);
  DataView &WriteNum(double val, size_t offset = 0);

  void ReadNum(unsigned char *val, size_t offset = 0) const;
  void ReadNum(char *val, size_t offset = 0) const;
  void ReadNum(unsigned *val, size_t offset = 0) const;
  void ReadNum(int *val, size_t offset = 0) const;
  void ReadNum(unsigned short *val, size_t offset = 0) const;
  void ReadNum(short *val, size_t offset = 0) const;
  void ReadNum(unsigned long long *val, size_t offset = 0) const;
  void ReadNum(long long *val, size_t offset = 0) const;
  void ReadNum(float *val, size_t offset = 0) const;
  void ReadNum(double *val, size_t offset = 0) const;

  template<typename T>
  T ReadNum(size_t offset = 0) const {
    T t;
    ReadNum(&t, offset);
    return t;
  }

//  const char *View() const { return buf_; }

//  char *View() { return buf_; }

 private:

  inline void writeBuffer(const char *data, size_t len, size_t offset) {
    memcpy(buf_ + offset, data, len);
  }

 private:
  char *buf_;
  EndianType endian_;
};

} // namespace lessdb