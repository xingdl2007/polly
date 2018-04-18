// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_TCP_CONNECTION_H_
#define NETWORK_POLLY_TCP_CONNECTION_H_

#include <memory>
#include <string>
#include "inetaddress.h"

namespace polly {

class EventLoop;
class Socket;
class Channel;
class TcpConnection;
class Buffer;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

// Weak callback
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
  using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
  using MessageCallback = std::function<void(const TcpConnectionPtr &,
                                             Buffer *,
                                             Timestamp)>;
  using CloseCallback = std::function<void(const TcpConnectionPtr &)>;

  // TCP connection state
  enum State { kConnecting, kConnected, kDisconnected };

public:
  TcpConnection(EventLoop *loop, std::string name, int conn_fd,
                const InetAddress &local, const InetAddress &remote);

  TcpConnection(TcpConnection const &) = delete;
  TcpConnection &operator=(TcpConnection const &) = delete;

  ~TcpConnection();

  std::string name() { return name_; }
  char *remoteIP() { return remote_addr_.IP(); }
  int remotePort() { return remote_addr_.Port(); }

  void HandleRead();
  void HandleWrite();
  void HandleClose();
  void HandleError();

  void SetConnectionCallback(const ConnectionCallback &cb) {
    conn_callback_ = cb;
  }
  void SetMessageCallback(const MessageCallback &cb) {
    msg_callback_ = cb;
  }
  void SetCloseCallback(const CloseCallback &cb) {
    close_callback_ = cb;
  }
  void ConnectEstablished();
  void ConnectDestroyed();

  // Send
  void Send(std::string const &msg);

  // Set state
  void SetState(State s) { state_ = s; }
  bool connected() { return state_ == kConnected; }

  // getLoop
  EventLoop *getLoop() { return loop_; }

private:

  EventLoop *loop_;
  std::string name_;
  State state_;

  // we don't expose those to client
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;

  InetAddress local_addr_;
  InetAddress remote_addr_;
  ConnectionCallback conn_callback_;
  MessageCallback msg_callback_;
  CloseCallback close_callback_;

  Buffer rev_buffer_; // receive buffer
  Buffer snd_buffer_; // send buffer
};

} // namespace polly

#endif //NETWORK_POLLY_TCP_CONNECTION_H_
