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

#include <boost/crc.hpp>

#include "BlockUtils.h"
#include "FileUtils.h"
#include "TableFormat.h"
#include "Status.h"
#include "DataView.h"
#include "Options.h"
#include "Block.h"

namespace lessdb {

// Read the block identified by "handle" from "file".  On failure return non-OK.
// On success read the data and return OK.
// NOTE: The Block pointer returned should be deleted when it's not needed.
inline Block *ReadBlockFromFile(RandomAccessFile *file,
                                const ReadOptions &options,
                                const Comparator *cmp,
                                const BlockHandle &handle, Status &s) {
  assert(handle.size > kBlockTrailerSize);

  std::unique_ptr<char[]> p_block_buf(new char[handle.size]);
  char *block_buf = p_block_buf.get();

  Slice data;
  s = file->Read(handle.size, handle.offset, block_buf, &data);

  if (s) {
    if (data.Len() < handle.size) {
      s = Status::Corruption("ReadBlockFromFile: Truncated block size");
    }
  }

  if (!s) {
    return nullptr;
  }

  uint64_t block_size = handle.size - kBlockTrailerSize;

  if (options.verify_checksums) {
    boost::crc_32_type crc32;

    crc32.process_bytes(block_buf, block_size);
    uint32_t actual_crc =
        ConstDataView(block_buf + block_size + sizeof(uint8_t))
            .ReadNum<uint32_t>();
    if (crc32.checksum() != actual_crc) {
      s = Status::Corruption("ReadBlockFromFile: Block checksum mismatch");
      return nullptr;
    }
  }

  BlockContent blck_content;
  blck_content.data = Slice(data.RawData(), data.Len() - kBlockTrailerSize);
  Block *ret = new Block(blck_content, cmp);

  p_block_buf.release();
  s = Status::OK();
  return ret;
}

}  // namespace lessdb