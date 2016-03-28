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

#include "Disallowcopying.h"
#include "Status.h"

namespace lessdb {

///
/// Interfaces under this file provide platform-independent file abstractions.
///

// A file abstraction for randomly reading the contents of a file.
class RandomAccessFile {
  __DISALLOW_COPYING__(RandomAccessFile);
 public:

  RandomAccessFile() = default;
  virtual ~RandomAccessFile() = default;

 public:

  // Read up to "n" bytes from the file starting at "offset".
  // "dst[0..n-1]" will be written by this routine.
  // Set "*result" to point at data in "dst[0..n-1]" (including
  // if fewer than "n" bytes were successfully read)),
  // so "dst[0..n-1]" must be live when "*result" is used.
  // If an error was encountered, returns a non-OK status.
  //
  // Safe for concurrent use by multiple threads.
  virtual Status Read(size_t n, uint64_t offset,
                      char *dst, Slice *result) = 0;
};

// A file abstraction for reading sequentially through a file.
class SequentialFile {
  __DISALLOW_COPYING__(SequentialFile);
 public:

  SequentialFile() = default;
  virtual ~SequentialFile() = default;

 public:

  // Read up to "n" bytes from the file.  "dst[0..n-1]" will be
  // written by this routine. Set "*result" to point at data
  // in "dst[0..n-1]"(including if fewer than "n" bytes were
  // successfully read)), so "scratch[0..n-1]" must be live
  // when "*result" is used.
  // If an error was encountered, returns a non-OK status.
  //
  // REQUIRES: External synchronization
  virtual Status Read(size_t n, char *dst, Slice *result) = 0;

  // Skip "n" bytes from the file. This is guaranteed to be no
  // slower that reading the same data, but may be faster.
  //
  // If end of file is reached, skipping will stop at the end of the
  // file, and Skip will return OK.
  //
  // REQUIRES: External synchronization
  virtual Status Skip(uint64_t n) = 0;
};

// A file abstraction for sequential writing.  The implementation
// must provide buffering since callers may append small fragments
// at a time to the file.
class WritableFile {
  __DISALLOW_COPYING__(WritableFile);
 public:

  WritableFile() = default;
  virtual ~WritableFile() = default;

 public:

  virtual Status Append(const Slice &data) = 0;

  virtual Status Sync() = 0;

  virtual Status Close() = 0;

  virtual Status Flush() = 0;
};

class FileFactory {
  __DISALLOW_COPYING__(FileFactory);
 public:

  FileFactory() = default;
  virtual ~FileFactory() = default;

 public:

  // Create a brand new random access read-only file with the specified name.
  // On success, stores OK in "*s" and returns a pointer to the new file.
  // On failure returns nullptr and stores non-OK in "*s".
  // If the file does not exist, stores a non-OK status.
  //
  // The returned file may be concurrently accessed by multiple threads.
  virtual RandomAccessFile *
      NewRandomAccessFile(const std::string &fname, Status *s) = 0;

  // Create a brand new sequentially-readable file with the specified name.
  // On success, stores OK in "*s" and returns a pointer to the new file.
  // On failure returns nullptr and stores non-OK in "*s".
  // If the file does not exist, stores a non-OK status.
  //
  // The returned file will only be accessed by one thread at a time.
  virtual SequentialFile *
      NewSequentialFile(const std::string &fname, Status *s) = 0;

  // Create a brand new sequentially-readable file with the specified name.
  // On success, stores OK in "*s" and returns a pointer to the new file.
  // On failure returns nullptr and stores non-OK in "*s".
  // If the file does not exist, stores a non-OK status.
  //
  // The returned file will only be accessed by one thread at a time.
  virtual WritableFile *
      NewWritableFile(const std::string &fname, Status *s) = 0;

  static FileFactory *Default();
};

} // namespace lessdb
