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


#include <folly/Varint.h>

#include "Slice.h"
#include "Coding.h"
#include "DataView.h"

namespace lessdb {

namespace coding {

using namespace folly;

void AppendFixed64(std::string *res, uint64_t v) {
  uint8_t buf[sizeof(uint64_t)];
  res->append(DataView(reinterpret_cast<char *>(buf)).WriteNum(v).View(),
              sizeof(uint64_t));
}

void AppendFixed32(std::string *res, uint32_t v) {
  uint8_t buf[sizeof(uint32_t)];
  res->append(DataView(reinterpret_cast<char *>(buf)).WriteNum(v).View(),
              sizeof(uint32_t));
}

void AppendVar64(std::string *res, uint64_t v) {
  uint8_t buf[kMaxVarintLength64];
  res->append(reinterpret_cast<char *>(buf), encodeVarint(v, buf));
}

void AppendVar32(std::string *res, uint32_t v) {
  uint8_t buf[kMaxVarintLength32];
  res->append(reinterpret_cast<char *>(buf), encodeVarint(v, buf));
}

bool GetFixed64(Slice *s, uint64_t *dest) {
  ConstDataView(s->RawData()).ReadNum(dest);
  s->Skip(sizeof(uint64_t));
  return true;
}

bool GetFixed32(Slice *s, uint32_t *dest) {
  ConstDataView(s->RawData()).ReadNum(dest);
  s->Skip(sizeof(uint32_t));
  return true;
}

bool GetVar64(Slice *s, uint64_t *dest) {
  StringPiece piece(s->RawData(), kMaxVarintLength64);
  (*dest) = decodeVarint(piece);
  assert(piece.data() - s->RawData() > 0);
  assert(piece.data() - s->RawData() <= s->Len());
  s->Skip(piece.data() - s->RawData());
  return true;
}

bool GetVar32(Slice *s, uint32_t *dest) {
  StringPiece piece(s->RawData(), kMaxVarintLength32);
  (*dest) = static_cast<uint32_t>(decodeVarint(piece));
  assert(piece.data() - s->RawData() > 0);
  assert(piece.data() - s->RawData() <= s->Len());
  s->Skip(piece.data() - s->RawData());
  return true;
}

void AppendVarString(std::string *res, const Slice &s) {
  AppendVar32(res, static_cast<uint32_t>(s.Len()));
  res->append(s.RawData(), s.Len());
}

Slice GetVarString(Slice *s) {
  uint32_t len;
  GetVar32(s, &len);
  Slice ret(s->RawData(), len);
  s->Skip(len);
  return ret;
}

} // namespace coding

} // namespace lessdb
