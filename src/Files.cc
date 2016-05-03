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

#include <string>
#include <system_error>
#include <sys/mman.h>
#include <fcntl.h>  // open
#include <folly/Likely.h>
#include <boost/filesystem.hpp>

#include "Files.h"

namespace lessdb {

static inline std::string ErrnoToString(int error_number) {
  return Slice(strerror(error_number)).ToString();
}

static inline Status FileError(const std::string &fname, int error_number) {
  return Status::IOError(ErrnoToString(error_number) + ": " + fname);
}

class PosixRandomAccessFile : public RandomAccessFile {
 public:
  // PosixRandomAccessFile doesn't create the connection to file in its
  // constructor, instead, the file descriptor representing the connection and
  // specified by fd, is initialized outside (by open(2)), so that the error
  // incurred by establishing the connection will not have to be handled in
  // constructor (though thereby we will have to manage resources lifetime in an
  // unsafe manner).
  //
  // See PosixFileFactory::NewRandomAccessFile
  PosixRandomAccessFile(const std::string &fname, int fd)
      : filename_(fname), fd_(fd) {}

  ~PosixRandomAccessFile() {
    close(fd_);
  }

  virtual Status Read(size_t n, uint64_t offset, char *dst,
                      Slice *result) override {
    ssize_t r = pread(fd_, dst, n, static_cast<off_t>(offset));
    *result = Slice(dst, static_cast<size_t>(r < 0 ? 0 : n));
    if (UNLIKELY(r < 0)) {
      return FileError(filename_, errno);
    }
    return Status::OK();
  }

 private:
  int fd_;                // the file descriptor
  std::string filename_;  // name of the file, used for error message(Status).
};

// Helper class to limit mmap file usage so that we do not end up
// running out virtual memory or running into kernel performance
// problems for very large databases.
//
// See PosixFileFactory::NewRandomAccessFile.
class MmapLimiter {
  __DISALLOW_COPYING__(MmapLimiter);

 public:
  // Up to 1000 mmaps for 64-bit binaries; none for smaller pointer sizes.
  MmapLimiter() : allowed_(1000) {}

  // If another mmap slot is available, acquire it and return true.
  // Else return false.
  bool Acquire() {
    // Avoid creating mutex lock if there's already no mmap available.
    if (allowed_.load(std::memory_order_acquire) <= 0)
      return false;

    std::lock_guard<std::mutex> guard(mu_);
    int v;
    // Check "allowed_", which may hold a different value with that of
    // the previous call.
    if ((v = allowed_.load(std::memory_order_acquire)) <= 0)
      return false;
    allowed_.store(v - 1, std::memory_order_release);
    return true;
  }

  // Release a slot acquired by a previous call to Acquire() that returned true.
  void Release() {
    std::lock_guard<std::mutex> guard(mu_);
    int v = allowed_.load(std::memory_order_acquire);
    allowed_.store(v + 1, std::memory_order_release);
  }

 private:
  std::atomic<int> allowed_;
  std::mutex mu_;
};

// mmap based random access
class PosixMmapReadableFile : public RandomAccessFile {
 public:
  // See PosixFileFactory::NewRandomAccessFile
  PosixMmapReadableFile(const std::string &fname, void *region, size_t length,
                        MmapLimiter *limiter)
      : filename_(fname),
        mmaped_region_(region),
        len_(length),
        limiter_(limiter) {}

  ~PosixMmapReadableFile() {
    limiter_->Release();
  }

  Status Read(size_t n, uint64_t offset, char *dst, Slice *result) override {
    // invalid argument
    assert(offset + n < len_);

    char *s = reinterpret_cast<char *>(mmaped_region_);
    memcpy(dst, s + offset, n);
    (*result) = Slice(dst);
    return Status::OK();
  }

 private:
  std::string filename_;
  void *mmaped_region_;
  size_t len_;

  // Used to release the resources hold by this mmap file.
  MmapLimiter *limiter_;
};

class PosixSequentialFile : public SequentialFile {
 public:
  // See FileFactory::NewSequentialFile
  PosixSequentialFile(const std::string &fname, FILE *f)
      : filename_(fname), file_(f) {}

  ~PosixSequentialFile() {
    fclose(file_);
  }

  Status Read(size_t n, char *dst, Slice *result) override {
    size_t r;
    if ((r = fread(dst, n, 1, file_)) == 0) {
      return FileError(filename_, errno);
    }
    (*result) = Slice(dst, r);
    return Status::OK();
  }

  Status Skip(uint64_t n) override {
    if (fseek(file_, static_cast<long>(n), SEEK_CUR) < 0) {
      return FileError(filename_, errno);
    }
    return Status::OK();
  }

 private:
  FILE *file_;
  std::string filename_;
};

class PosixWritableFile : public WritableFile {
 public:
  // See FileFactory::NewWritableFile
  PosixWritableFile(const std::string &fname, FILE *f)
      : filename_(fname), file_(f) {}

  virtual ~PosixWritableFile() override {
    if (!file_) {
      fclose(file_);
      file_ = nullptr;
    }
  }

  virtual Status Sync() override {
    if (!fflush(file_)) {
      // use fdatasync on linux
      if (!fsync(fileno(file_))) {
        return Status::OK();
      }
    }
    return FileError(filename_, errno);
  }

  virtual Status Append(const Slice &data) override {
    // If the return number of fwrite differs from data.Len(),
    // a writing error prevented fwrite from completing.
    // Use fwrite_unlocked on linux.
    if (fwrite(data.RawData(), 1, data.Len(), file_) != data.Len())
      return FileError(filename_, errno);
    return Status::OK();
  }

  virtual Status Flush() override {
    if (fflush(file_) != 0) {
      // flush_unlocked is exclusive on linux
      return FileError(filename_, errno);
    }
    return Status::OK();
  }

  virtual Status Close() override {
    if (fclose(file_) != 0) {
      return FileError(filename_, errno);
    }
    file_ = nullptr;
    return Status::OK();
  }

 private:
  FILE *file_;
  std::string filename_;
};

class PosixFileFactory : public FileFactory {
 public:
  virtual RandomAccessFile *NewRandomAccessFile(const std::string &fname,
                                                Status *s) override {
    int fd = open(fname.c_str(), O_RDONLY);

    if (fd < 0) {
      *s = FileError(fname, errno);
      return nullptr;
    }

    if (pLimiter_->Acquire()) {
      boost::system::error_code ec;
      void *region = nullptr;
      uintmax_t size = boost::filesystem::file_size(fname, ec);

      if (LIKELY(!ec)) {
        region = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
        if (region == MAP_FAILED) {
          *s = FileError(fname, errno);
        }
      } else {
        *s = Status::IOError(fname + ": " + ec.message());
      }

      close(fd);
      if (!s->IsOK()) {
        pLimiter_->Release();
        return nullptr;
      }

      return new PosixMmapReadableFile(fname, region, size, pLimiter_.get());
    }

    *s = Status::OK();
    return new PosixRandomAccessFile(fname, fd);
  }

  SequentialFile *NewSequentialFile(const std::string &fname,
                                    Status *s) override {
    FILE *f = fopen(fname.c_str(), "r");
    if (UNLIKELY(f == nullptr)) {
      *s = FileError(fname, errno);
      return nullptr;
    }
    *s = Status::OK();
    return new PosixSequentialFile(fname, f);
  }

  WritableFile *NewWritableFile(const std::string &fname, Status *s) override {
    FILE *f = fopen(fname.c_str(), "r");
    if (UNLIKELY(f == nullptr)) {
      *s = FileError(fname, errno);
      return nullptr;
    }
    *s = Status::OK();
    return new PosixWritableFile(fname, f);
  }

 private:
  // Used to limit mmap file usage.
  std::unique_ptr<MmapLimiter> pLimiter_;
};

FileFactory *FileFactory::Default() {
  static std::once_flag flag;
  static PosixFileFactory *instance_ = nullptr;
  std::call_once(flag, [] { instance_ = new PosixFileFactory(); });
  return instance_;
}

}  // namespace lessdb