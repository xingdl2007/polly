// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <sys/syscall.h>
#include "current_thread.h"

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
} // namespace polly