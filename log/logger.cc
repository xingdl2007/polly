// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// DefaultDestination() send all logs to stdout.

#include "logger.h"
#include "include/polly/slice.h"
#include "util/current_thread.h"

// global log level control
polly::LogLevel gLogLevel = polly::LogLevel::INFO;

// getter and setter
polly::LogLevel getLogLevel() { return gLogLevel; }
void setLogLevel(polly::LogLevel level) { gLogLevel = level; }

// logs' default destination: stdout
void DefaultDestination(polly::Slice const &s) {
  fputs(s.data(), stdout);
}

namespace polly {

const char *LogLevelName[LogLevel::NUM_LOG_LEVELS] = {
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

Logger::Destination Logger::func_ = DefaultDestination;

Logger::Logger(SourceFile const &file, int line, LogLevel level)
    : file_(file), line_(line), level_(level) {

  // timestamp: date + time
  time_ = TimeStamp::now();
  stream_ << time_.toFormatedString(true) << "Z ";

  // thread id
  stream_ << this_thread::tid() << " ";

  // log level
  stream_ << LogLevelName[level_];
}

Logger::~Logger() {
  // filename + line
  stream_ << " - " << Slice(file_.data_, file_.size_) << " " << line_ << "\n";
  func_(stream_.slice());
}

} // namespace polly
