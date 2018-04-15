// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Logger is responsible for a single log message and should not use directly.
// Instead, LOG_XXX macros construct temp Logger, destruction of Logger will
// send the single log message to destination (default stdout).

#ifndef NETWORK_POLLY_LOG_LOGGING_H_
#define NETWORK_POLLY_LOG_LOGGING_H_

#include <util/timestamp.h>
#include "log_stream.h"
#include "util/timestamp.h"

namespace polly {

enum LogLevel {
  TRACE = 0,
  DEBUG,
  INFO,
  WARN,
  ERROR,
  FATAL,
  NUM_LOG_LEVELS,
};

class Logger {
  class SourceFile {
  public:
    template<size_t N>
    inline SourceFile(const char (&arr)[N]): data_(arr),
                                             size_(N - 1) {
      const char *slash = strrchr(data_, '/'); // builtin function
      if (slash) {
        data_ = slash + 1;
        size_ -= static_cast<size_t>(data_ - arr);
      }
    }
    explicit SourceFile(const char *filename) : data_(filename) {
      const char *slash = strrchr(filename, '/');
      if (slash) {
        data_ = slash + 1;
      }
      size_ = static_cast<int>(strlen(data_));
    }
    const char *data_;
    size_t size_;
  };

  typedef void (*Destination)(Slice const &);

public:
  Logger(SourceFile const &file, int line, LogLevel level);

  Logger(Logger const &) = delete;
  Logger &operator=(Logger const &) = delete;

  ~Logger();

  LogStream &stream() { return stream_; }

  static Destination destination() { return func_; }
  static void destination(Destination d) { func_ = d; }

private:
  LogStream stream_;
  SourceFile file_;
  int line_;
  TimeStamp time_;
  LogLevel level_;

  static Destination func_;
};

} // namespace polly

extern polly::LogLevel gLogLevel;
polly::LogLevel getLogLevel();
void setLogLevel(polly::LogLevel level);

#define LOG_TRACE if(gLogLevel <= polly::LogLevel::TRACE) \
                  polly::Logger(__FILE__, __LINE__, LogLevel::TRACE).stream()
#define LOG_DEBUG if(gLogLevel <= polly::LogLevel::DEBUG) \
                  polly::Logger(__FILE__, __LINE__, LogLevel::DEBUG).stream()
#define LOG_INFO if(gLogLevel <= polly::LogLevel::INFO) \
                  polly::Logger(__FILE__, __LINE__, LogLevel::INFO).stream()

#define LOG_WARNING polly::Logger(__FILE__, __LINE__, LogLevel::WARN).stream()
#define LOG_ERROR polly::Logger(__FILE__, __LINE__, LogLevel::ERROR).stream()
#define LOG_FATAL polly::Logger(__FILE__, __LINE__, LogLevel::FATAL).stream()

#endif //NETWORK_POLLY_LOG_LOGGING_H_
