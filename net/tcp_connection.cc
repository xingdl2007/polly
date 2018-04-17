// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <unistd.h>
#include "eventloop.h"
#include "tcp_connection.h"
#include "socket.h"
#include "channel.h"
#include "log/logger.h"

namespace polly {

TcpConnection::TcpConnection(EventLoop *loop, std::string name, int conn_fd, const InetAddress remote)
    : loop_(loop), name_(name), socket_(std::make_unique<Socket>(conn_fd)),
      channel_(std::make_unique<Channel>(loop, conn_fd)), remote_addr_(remote) {

  channel_->SetReadCallback([this]() {
    this->HandleRead();
  });
  channel_->EnableReading();
}

void TcpConnection::HandleRead() {
  LOG_TRACE << "TcpConnection::HandleRead()";
  char data[65536];

  ssize_t n = ::read(channel_->fd(), data, sizeof data);
  Buffer buf(data, n);

  LOG_TRACE << "TcpConnection::onMessage()";
  if (n > 0) {
    if (msg_callback_) {
      msg_callback_(shared_from_this(), &buf, Timestamp::now());
    }
  } else if (n == 0) {
    HandleClose();
  } else {
    HandleError();
  }
}

void TcpConnection::HandleWrite() {

}

void TcpConnection::HandleClose() {
  loop_->assertInLoopThread();
  LOG_TRACE << "TcpConnection::HandleClose()";

  // we don't close fd, leave it to dtor, so we can find leaks easily.
  channel_->DisbaleAll();
  if (close_callback_) {
    close_callback_(shared_from_this());
  }
}

// only log
void TcpConnection::HandleError() {
  LOG_ERROR << "TcpConnection::HandleError()";
}

void TcpConnection::ConnectEstablished() {
  if (conn_callback_) {
    conn_callback_(shared_from_this());
  }
}

} // namespace polly