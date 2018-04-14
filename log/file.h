// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// AppendFile is an abstraction of a file on disk, only append operation
// is allowed. This class is thread unsafe with the assumption that only
// one thread can write to the underlying file.

#ifndef NETWORK_POLLY_LOG_FILE_H_
#define NETWORK_POLLY_LOG_FILE_H_

#include <stdio.h>
#include <string>
#include "polly/slice.h"

namespace polly {

class AppendFile {
public:
  AppendFile(std::string const &file) : has_written_(0) {
    fd_ = ::fopen(file.c_str(), "ae"); // a: appending; e: O_CLOEXEC
    ::setbuffer(fd_, buffer, sizeof buffer);
  }

  AppendFile(AppendFile const &) = delete;
  AppendFile &operator=(AppendFile const) = delete;

  ~AppendFile() {
    ::fclose(fd_);
  }
  // use fwrite_unlocked instead of fwrite for efficiency
  void Append(const Slice &s);

  off64_t HasWritten() const { return has_written_; }
  void Flush() {
    ::fflush(fd_);
  }
private:
  FILE *fd_;
  char buffer[64 * 4096]; // block buffered
  off64_t has_written_;
};

} // namespace polly

#endif //NETWORK_POLLY_LOG_FILE_H_
