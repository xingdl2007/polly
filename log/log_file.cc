// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// LogFile owns an AppendFile all the time. It will roll to another either
// when current file is too big or in the new day.

#include "log_file.h"

namespace polly {

LogFile::LogFile(std::string const &file) : basename_(file) {
  rollToNewFile();
}

void LogFile::Append(const Slice &s) {
  static int count = 0;
  file_->Append(s);

  if (file_->HasWritten() > kRollingSize) {
    rollToNewFile();
  } else {
    ++count;

    // check per 1024 logs
    if (count >= kCheckInterval) {
      count = 0;
      time_t now = time(nullptr);
      time_t thisPeriod_ = now / kRollPerDay_ * kRollPerDay_;

      // a new day?
      if (thisPeriod_ != start_period_) {
        rollToNewFile();
      } else if (now - last_flush_ > kFlushInterval) {
        last_flush_ = now;
        file_->Flush();
      }
    }
  }
}

void LogFile::rollToNewFile() {
  time_t now;
  auto filename = getLogFileName(basename_, &now);
  assert(now > last_roll_);

  last_roll_ = now;
  last_flush_ = now;
  start_period_ = now / kRollPerDay_ * kRollPerDay_;
  file_ = std::make_unique<AppendFile>(filename);
}

// format:
std::string LogFile::getLogFileName(const std::string &basename, time_t *now) {
  *now = time(nullptr);
  return "";
}

} // namespace polly