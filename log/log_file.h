// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_LOG_LOG_FILE_H_
#define NETWORK_POLLY_LOG_LOG_FILE_H_

#include <time.h>
#include <memory>
#include <mutex>
#include "file.h"

namespace polly {

// thread unsafe
class LogFile {
public:
  explicit LogFile(std::string const &f);

  LogFile(LogFile const &) = delete;
  LogFile &operator=(LogFile const &) = delete;

  ~LogFile() = default;

  void Append(Slice const &s);

private:
  void rollToNewFile();
  static std::string getLogFileName(const std::string &basename, time_t *now);

  const std::string basename_;

  std::unique_ptr<AppendFile> file_;

  time_t last_roll_;
  time_t last_flush_;
  time_t start_period_;

  const static off64_t kRollingSize = 100 * 1024 * 1024; // bytes, 100MB
  const static int kFlushInterval = 5;                   // seconds
  const static int kCheckInterval = 1024;                // times
  const static int kRollPerDay_ = 60 * 60 * 24;          // 1 day
};

} // namespace polly

#endif // NETWORK_POLLY_LOG_LOG_FILE_H_
