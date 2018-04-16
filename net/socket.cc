// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <stdlib.h>
#include <sys/socket.h>
#include "socket.h"
#include "log/logger.h"

namespace polly {

void Socket::Bind(InetAddress const &addr_) {
  auto addr = addr_.SockAddress();
  int ret = ::bind(sockfd_, reinterpret_cast<sockaddr *> (&addr), sizeof addr);
  if (ret == -1) {
    LOG_FATAL << "Socket::Bind() failed, port: " << addr_.Port();
    ::abort();
  }
}

// only support IPv4 + TCP for simplicity
int NewNonBlockingSocket() {
  int sockfd = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
  if (sockfd == -1) {
    LOG_FATAL << "NewNonBlockingSocket() failed";
    ::abort();
  }
  return sockfd;
}

} // namespace polly
