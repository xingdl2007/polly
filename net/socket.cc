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

void Socket::ShutdownWrite() {
  if (::shutdown(sockfd_, SHUT_WR) < 0) {
    LOG_ERROR << "Socket::ShutdownWrite()";
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

struct sockaddr_in getLocalAddr(int sockfd) {
  struct sockaddr_in localaddr;
  bzero(&localaddr, sizeof localaddr);
  socklen_t addrlen = sizeof localaddr;
  if (::getsockname(sockfd, reinterpret_cast<sockaddr *>(&localaddr), &addrlen) < 0) {
    LOG_ERROR << "sockets::getLocalAddr";
  }
  return localaddr;
}

struct sockaddr_in getPeerAddr(int sockfd) {
  struct sockaddr_in peeraddr;
  bzero(&peeraddr, sizeof peeraddr);
  socklen_t addrlen = sizeof peeraddr;
  if (::getpeername(sockfd, reinterpret_cast<sockaddr *>(&peeraddr), &addrlen) < 0) {
    LOG_ERROR << "sockets::getPeerAddr";
  }
  return peeraddr;
}

} // namespace polly
