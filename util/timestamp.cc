// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Borrowed from muduo:
//
// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include <unistd.h>
#include <sys/time.h>
#include <inttypes.h>
#include "timestamp.h"

namespace polly {

// second + microsecond
std::string Timestamp::toString() const {
  char buffer[32] = {0};
  int64_t seconds = micro_since_epoch_ / kMicroSecondsPerSecond;
  int64_t micro = micro_since_epoch_ % kMicroSecondsPerSecond;
  snprintf(buffer, sizeof(buffer) - 1, "%" PRId64 ".%06" PRId64, seconds, micro);
  return buffer;
}

std::string Timestamp::toFormatedString(bool showMicroseconds) const {

  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(micro_since_epoch_ / kMicroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  if (showMicroseconds) {
    int microseconds = static_cast<int>(micro_since_epoch_ % kMicroSecondsPerSecond);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
             microseconds);
  } else {
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
  }
  return buf;
}

Timestamp Timestamp::now() {
  struct timeval t;
  ::gettimeofday(&t, nullptr);
  return Timestamp(t.tv_sec * kMicroSecondsPerSecond + t.tv_usec);
}

} // namespace polly