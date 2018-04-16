// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_NET_SOCKET_H_
#define NETWORK_POLLY_NET_SOCKET_H_

#include <unistd.h>
#include "inetaddress.h"

namespace polly {

class Socket {
public:
  explicit Socket(int fd) : sockfd_(fd) {}

  Socket(Socket const &) = delete;
  Socket &operator=(Socket const &) = delete;

  ~Socket() { ::close(sockfd_); }

  void Bind(InetAddress const &);
  int fd() const { return sockfd_; }

private:
  const int sockfd_;
};

// non-blocking socket
int NewNonBlockingSocket();

} // namespace polly

#endif //NETWORK_POLLY_NET_SOCKET_H_
