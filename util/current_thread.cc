// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <sys/syscall.h>
#include "current_thread.h"

namespace polly {
namespace this_thread {

// system call
pid_t gettid() {
  return static_cast<pid_t >(::syscall(__NR_gettid));
}

} // namespace this_thread
} // namespace polly