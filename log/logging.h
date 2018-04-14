// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_LOG_LOGGING_H_
#define NETWORK_POLLY_LOG_LOGGING_H_

#include "log_stream.h"

namespace polly {

enum class LogLevel {
  TRACE,
  DEBUG,
  INFO,
  WARN,
  ERROR,
  FATAL,
  NUM_LOG_LEVELS,
};

class Logging {
public:
  Logging() = default;

  Logging(Logging const &) = delete;
  Logging &operator=(Logging const &) = delete;

  ~Logging() {
    fputs(stream_.data(), stdout);
  }

  LogStream &stream() { return stream_; }

private:
  LogStream stream_;
};

} // namespace polly

#define LOG_TRACE polly::Logging().stream()
#define LOG_DEBUG polly::Logging().stream()
#define LOG_INFO polly::Logging().stream()
#define LOG_WARNING polly::Logging().stream()
#define LOG_ERROR polly::Logging().stream()
#define LOG_FATAL polly::Logging().stream()

#endif //NETWORK_POLLY_LOG_LOGGING_H_
