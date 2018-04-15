// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// AppendFile is an abstraction of a file on disk, only append operation
// is allowed. This class is thread unsafe with the assumption that only
// one thread can write to the underlying file.

#include "include/polly/slice.h"
#include "file.h"

namespace polly {

// use fwrite_unlocked instead of fwrite for efficiency
void AppendFile::Append(const Slice &s) {
  if (s.data() == nullptr || s.size() == 0) {
    return;
  }
  auto written = ::fwrite_unlocked(s.data(), 1, s.size(), fd_);
  auto remaining = s.size() - written;

  while (remaining > 0) {
    auto n = ::fwrite_unlocked(s.data() + written, 1, s.size() - written, fd_);
    if (n == 0) {
      int err = ferror(fd_);
      if (err) {
        //fprintf(stderr, "AppendFile::Append() failed %s\n", strerror_tl(err));
      }
      break;
    }
    written += n;
    remaining -= n;
  }
  has_written_ += s.size();
}

} // namespace polly
