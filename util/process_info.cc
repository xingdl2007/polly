// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <unistd.h>
#include "process_info.h"

namespace polly {
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