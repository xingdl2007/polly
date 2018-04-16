// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "acceptor.h"
#include "socket.h"
#include "log/logger.h"

namespace polly {

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listen_addr)
    : loop_(loop), listen_socket_(NewNonBlockingSocket()),
      listen_channel_(loop, listen_socket_.fd()) {

  // bind address
  listen_socket_.Bind(listen_addr);
  // register callback
  listen_channel_.SetReadCallback([&]() {
    this->handleRead();
  });
  listen_channel_.EnableReading();

  listen();
}

void Acceptor::listen() {
  int ret = ::listen(listen_socket_.fd(), SOMAXCONN);
  if (ret == -1) {
    LOG_FATAL << "Acceptor::Listen() failed";
    ::abort();
  }
}

void Acceptor::handleRead() {
  sockaddr_in addr_ = {};
  socklen_t len = sizeof addr_;
  int connfd = ::accept4(listen_socket_.fd(), reinterpret_cast<sockaddr *>(&addr_),
                         &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (connfd < 0) {
    LOG_FATAL << "Acceptor::handleRead() failed";
    ::abort();
  }

  LOG_TRACE << "new connection " << connfd << ", ip: " << addr_.sin_addr.s_addr
            << " port: " << addr_.sin_port;

  // new Connection callback
  InetAddress client(addr_);
  if (new_conn_callback_) {
    new_conn_callback_(connfd, client);
  } else {
    ::close(connfd);
  }
}

} // namespace polly
