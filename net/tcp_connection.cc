// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "tcp_connection.h"
#include "socket.h"
#include "channel.h"

namespace polly {

TcpConnection::TcpConnection(EventLoop *loop, std::string name, int conn_fd, const InetAddress remote)
    : loop_(loop), name_(name), socket_(std::make_unique<Socket>(conn_fd)),
      channel_(std::make_unique<Channel>(loop, conn_fd)), remote_addr_(remote) {

}

} // namespace polly