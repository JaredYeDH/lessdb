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

#include <boost/endian/buffers.hpp>

#include "DataView.h"

using namespace boost::endian;

namespace lessdb {

// DataView uses boost::endian library to converse the endian of
// integral numbers. (boost::endian doesn't provide endian
// conversion for float point numbers, but we can easily implement
// it.)

#define WRITE_NUM_IMPL(data_type) \
  if (endian_ == kLittleEndian) { \
    little_##data_type##_buf_t t(static_cast<data_type##_t>(val)); \
    writeBuffer(t.data(), sizeof(data_type##_t), offset); \
  } else if (endian_ == kBigEndian) { \
    big_##data_type##_buf_t t(static_cast<data_type##_t >(val)); \
    writeBuffer(t.data(), sizeof(data_type##_t), offset); \
  } else { \
    char *t = reinterpret_cast<char *>(&val); \
    writeBuffer(t, sizeof(data_type##_t), offset); \
  } \
  return (*this)


DataView &DataView::WriteNum(unsigned char val, size_t offset) {
  WRITE_NUM_IMPL(uint8);
}

DataView &DataView::WriteNum(char val, size_t offset) {
  static_assert(CHAR_BIT == 8, "CHAR_BIT == 8");
  WRITE_NUM_IMPL(int8);
}

DataView &DataView::WriteNum(unsigned short val, size_t offset) {
  WRITE_NUM_IMPL(uint16);
}

DataView &DataView::WriteNum(short val, size_t offset) {
  static_assert(sizeof(short) == 2, "sizeof(short) == 2");
  WRITE_NUM_IMPL(int16);
}

DataView &DataView::WriteNum(unsigned val, size_t offset) {
  WRITE_NUM_IMPL(uint32);
}

DataView &DataView::WriteNum(int val, size_t offset) {
  static_assert(sizeof(int) == 4, "sizeof(int) == 4");
  WRITE_NUM_IMPL(int32);
}

DataView &DataView::WriteNum(unsigned long long val, size_t offset) {
  WRITE_NUM_IMPL(uint64);
}

DataView &DataView::WriteNum(long long val, size_t offset) {
  static_assert(sizeof(long long) == 8, "sizeof(long long) == 8");
  WRITE_NUM_IMPL(int64);
}

#undef WRITE_NUM_IMPL

DataView &DataView::WriteNum(float val, size_t offset) {
  static_assert(sizeof(float) == 4, "sizeof(float) == 4");
  uint32_t temp;
  memcpy(&temp, &val, sizeof(val));
  WriteNum(temp, offset);
  return (*this);
}

DataView &DataView::WriteNum(double val, size_t offset) {
  static_assert(sizeof(double) == 8, "sizeof(double) == 8");
  uint64_t temp;
  memcpy(&temp, &val, sizeof(val));
  WriteNum(temp, offset);
  return (*this);
}

#define READ_NUM_IMPL(data_type) \
  memcpy(val, buf_ + offset, sizeof(data_type##_t)); \
  if (endian_ == EndianType::kLittleEndian) { \
    little_to_native_inplace(*val); \
  } else if (endian_ == EndianType::kBigEndian) { \
    big_to_native_inplace(*val); \
  }


void ConstDataView::ReadNum(unsigned char *val, size_t offset) const {
  READ_NUM_IMPL(uint8);
}

void ConstDataView::ReadNum(char *val, size_t offset) const {
  READ_NUM_IMPL(int8);
}

void ConstDataView::ReadNum(unsigned *val, size_t offset) const {
  READ_NUM_IMPL(uint32);
}

void ConstDataView::ReadNum(int *val, size_t offset) const {
  READ_NUM_IMPL(int32);
}

void ConstDataView::ReadNum(unsigned short *val, size_t offset) const {
  READ_NUM_IMPL(uint16);
}

void ConstDataView::ReadNum(short *val, size_t offset) const {
  READ_NUM_IMPL(int16);
}

void ConstDataView::ReadNum(unsigned long long *val, size_t offset) const {
  READ_NUM_IMPL(uint64);
}

void ConstDataView::ReadNum(long long *val, size_t offset) const {
  READ_NUM_IMPL(int64);
}

void ConstDataView::ReadNum(float *val, size_t offset) const {
  uint32_t temp;
  ReadNum(&temp, offset);
  memcpy(val, &temp, sizeof(float));
}

void ConstDataView::ReadNum(double *val, size_t offset) const {
  uint64_t temp;
  ReadNum(&temp, offset);
  memcpy(val, &temp, sizeof(double));
}

} // namespace lessdb