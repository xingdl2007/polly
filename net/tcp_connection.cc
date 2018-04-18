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

TcpConnection::TcpConnection(EventLoop *loop, std::string name, int conn_fd,
                             const InetAddress &local, const InetAddress &remote)
    : loop_(loop), name_(name), state_(kConnecting),
      socket_(std::make_unique<Socket>(conn_fd)),
      channel_(std::make_unique<Channel>(loop, conn_fd)),
      local_addr_(local), remote_addr_(remote) {
  // read
  channel_->SetReadCallback([this]() { this->HandleRead(); });
  channel_->EnableReading();
  // write
  channel_->SetWriteCallback([this]() { this->HandleWrite(); });
  // close: unlike read/write, close does not need enable
  channel_->SetCloseCallback([this]() { this->HandleClose(); });
}

TcpConnection::~TcpConnection() {
  assert(state_ == kDisconnected);
  LOG_DEBUG << "TcpConnection::~TcpConnection() " << name_
            << ", fd = " << channel_->fd();
}

void TcpConnection::HandleRead() {
  LOG_INFO << "TcpConnection::HandleRead()";
  ssize_t n = rev_buffer_.readFd(channel_->fd(), nullptr);

  if (n > 0) {
    LOG_INFO << "TcpConnection::onMessage()";
    if (msg_callback_) {
      msg_callback_(shared_from_this(), &rev_buffer_, Timestamp::now());
    }
  } else if (n == 0) {
    HandleClose();
  } else {
    HandleError();
  }
}

void TcpConnection::HandleClose() {
  loop_->assertInLoopThread();
  LOG_INFO << "TcpConnection::HandleClose()";

  assert(state_ == kConnected || state_ == kDisconnecting);
  channel_->DisbaleAll();
  SetState(kDisconnected);

  // clean up TcpServer
  if (close_callback_) {
    close_callback_(shared_from_this());
  }
}

// only log
void TcpConnection::HandleError() {
  LOG_ERROR << "TcpConnection::HandleError()";
}

void TcpConnection::ConnectEstablished() {
  SetState(kConnected);
  if (conn_callback_) {
    conn_callback_(shared_from_this());
  }
}

// 1. call before TcpConnection is destructing, notify user.
//    the last function before die.
// 2. called when owner (TcpServer/TcpClient) destroyed, if
//    there is connection still kConnected.
void TcpConnection::ConnectDestroyed() {
  loop_->assertInLoopThread();
  LOG_INFO << "TcpConnection::ConnectDestroyed()";

  // if die abnormality, state_ will still be kConnected
  if (state_ == kConnected || state_ == kDisconnecting) {
    channel_->DisbaleAll();
  }
  SetState(kDisconnected);

  // notify user
  if (conn_callback_) {
    conn_callback_(shared_from_this());
  }
}

// Send can be called in non-IO thread, which is useful
void TcpConnection::Send(std::string const &msg) {
  if (loop_->isInLoopThread()) {
    sendInLoop(msg);
  } else {
    loop_->RunInLoop([&]() { this->sendInLoop(msg); });
  }
}

// interesting, will be called in IO thread
void TcpConnection::sendInLoop(std::string const &msg) {
  loop_->assertInLoopThread();
  ssize_t nwrote = 0;

  // if nothing in output queue, try writing directly
  if (!channel_->isWritingEnabled() && snd_buffer_.readableBytes() == 0) {
    nwrote = ::write(channel_->fd(), msg.data(), msg.size());
    if (nwrote >= 0) {
      if (static_cast<size_t >(nwrote) < msg.size()) {
        LOG_TRACE << "I am going to write more data";
      } else {
        if (write_complete_callback_) {
          loop_->RunInLoop(std::bind(write_complete_callback_,
                                     shared_from_this()));
        }
      }
    } else {
      nwrote = 0;
      if (errno != EWOULDBLOCK) {
        LOG_ERROR << "TcpConnection::Send() failed";
        return;
      }
    }
  }
  // buffer left data to snd_buffer_ if any
  assert(nwrote >= 0);
  if (static_cast<size_t >(nwrote) < msg.size()) {
    LOG_TRACE << "buffer left data in send buffer";
    snd_buffer_.append(msg.data() + nwrote, msg.size() - nwrote);
    if (!channel_->isWritingEnabled()) {
      channel_->EnableWriting();
    }
  }
}

// Shutdown can be called in non-IO thread, which is useful
void TcpConnection::Shutdown() {
  if (state_ == kConnected) {
    SetState(kDisconnecting);
    loop_->RunInLoop([&]() { this->shutdownInLoop(); });
  }
}

// close write-end, will be called in IO thread
void TcpConnection::shutdownInLoop() {
  loop_->assertInLoopThread();
  // may be sending data, so postpone
  if (!channel_->isWritingEnabled()) {
    socket_->ShutdownWrite();
  }
}

void TcpConnection::HandleWrite() {
  loop_->assertInLoopThread();
  if (channel_->isWritingEnabled()) {
    ssize_t n = ::write(channel_->fd(), snd_buffer_.peek(),
                        snd_buffer_.readableBytes());
    if (n > 0) {
      snd_buffer_.retrieve(n);
      if (snd_buffer_.readableBytes() == 0) {
        channel_->DisableWriting();
        if (write_complete_callback_) {
          loop_->RunInLoop(std::bind(write_complete_callback_,
                                     shared_from_this()));
        }
        // continuing shutdown, after send all data out
        if (state_ == kDisconnecting) {
          shutdownInLoop();
        }
      } else {
        LOG_TRACE << "I am going to write more data";
      }
    } else {
      LOG_TRACE << "TcpConnection::HandleWrite()";
    }
  } else {
    LOG_TRACE << "Connection is donw, no more writing.";
  }
}

} // namespace polly