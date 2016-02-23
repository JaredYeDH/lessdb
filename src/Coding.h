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

#include <iosfwd>
#include <cstdint>

namespace lessdb {

class Slice;

// coding is a group of utility routines that encode integer and
// character string in a portable binary format.

namespace coding {

// Fixed64 and Fixed32 are both Fixed-length integer encoding.
//
// Var64 and Var32 are variable-length integer encoding, using
// the format of varint in google/protobuf, find details in
// https://developers.google.com/protocol-buffers/docs/encoding#varints
//
// All of the encodings above are using little-endian representation.

// Append... routines append the value of v to string res.
extern void AppendFixed64(std::string *res, uint64_t v);
extern void AppendFixed32(std::string *res, uint32_t v);
extern void AppendVar64(std::string *res, uint64_t v);
extern void AppendVar32(std::string *res, uint32_t v);

// Get... routines parse a value from the beginning of s,
// and advances the slice past the parsed value.
extern bool GetFixed64(Slice *s, uint64_t *dest);
extern bool GetFixed32(Slice *s, uint32_t *dest);

// @throws std::invalid_argument
extern bool GetVar64(Slice *s, uint64_t *dest);
extern bool GetVar32(Slice *s, uint32_t *dest);

// varstring :=
//    len:  varint32
//    data: uint8[len]
extern void AppendVarString(std::string *res, const Slice &s);

// @throws std::invalid_argument
extern bool GetVarString(Slice *s, Slice *dest);

} // namespace coding

} // namespace lessdb