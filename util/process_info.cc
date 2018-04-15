// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <unistd.h>
#include <sys/syscall.h>
#include "process_info.h"

namespace polly {
namespace this_thread {

thread_local pid_t current_tid_ = 0;

// system call
pid_t tid() {
  if (current_tid_ == 0) {
    current_tid_ = static_cast<pid_t>(::syscall(__NR_gettid));
  }
  return current_tid_;
}

} // namespace this_thread

namespace process {

pid_t pid() {
  return ::getpid();
}

std::string pidString() {
  char buf[32];
  snprintf(buf, sizeof buf, "%d", pid());
  return buf;
}

std::string hostname() {
  // _POSIX_HOST_NAME_MAX 255
  char buffer[256];
  if (::gethostname(buffer, sizeof buffer) == 0) {
    buffer[sizeof buffer - 1] = '\0';
    return buffer;
  } else {
    return "unknown";
  }
}

} // namespace process
} // namespace polly