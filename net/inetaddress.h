// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_NET_INETADDRESS_H_
#define NETWORK_POLLY_NET_INETADDRESS_H_

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "log/logger.h"

namespace polly {

// only support IPv4 for simplicity, and does not support hostname (DNS resolve)
// e.g. "192.168.1.1" + 5001
class InetAddress {
public:
  InetAddress() = default;

  InetAddress(const char *ip, int port) {
    LOG_TRACE << "ip: " << ip << " port: " << port;
    ::bzero(&addr_, sizeof addr_);
    addr_.sin_family = AF_INET;
    inet_aton(ip, &addr_.sin_addr);
    addr_.sin_port = htons(port);
  }

  explicit InetAddress(const sockaddr_in &add) : addr_(add) {};

  sockaddr_in SockAddress() const { return addr_; }
  char *IP() const { return inet_ntoa(addr_.sin_addr); }
  int Port() const { return ntohs(addr_.sin_port); }
  socklen_t Size() const { return sizeof addr_; }

private:
  sockaddr_in addr_;
};

} // namespace polly

#endif //NETWORK_POLLY_NET_INETADDRESS_H_
