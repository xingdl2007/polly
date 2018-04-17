// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_NET_TCP_SERVER_H_
#define NETWORK_POLLY_NET_TCP_SERVER_H_

#include <string>
#include <memory>
#include <functional>
#include <map>
#include "inetaddress.h"
#include "tcp_connection.h"

namespace polly {

class EventLoop;
class Acceptor;
class Buffer;

class TcpServer {
  using ConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
  using MessageCallback = std::function<void(const TcpConnectionPtr &,
                                             Buffer *,
                                             Timestamp)>;
public:
  TcpServer(EventLoop *loop, const InetAddress &listen_addr, std::string name);

  TcpServer(TcpServer const &) = delete;
  TcpServer &operator=(TcpServer const &) = delete;

  ~TcpServer();

  // Start the server if it's not listening.
  void Start();

  // Set connection callback.
  void SetConnectionCallback(const ConnectionCallback &cb) {
    conn_callback_ = cb;
  }
  // Set message callback.
  void SetMessageCallback(const MessageCallback &cb) {
    msg_callback_ = cb;
  }

private:
  typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

  EventLoop *loop_;                    // the acceptor loop
  const std::string name_;             // server name
  std::unique_ptr<Acceptor> acceptor_; // listen socket
  ConnectionCallback conn_callback_;   // pass to every tcp connection
  MessageCallback msg_callback_;       // pass to every tcp connection
  int next_conn_id_;                   // for assign unique name to client
  ConnectionMap connections_;          // management of all connections
};

} // namespace polly

#endif //NETWORK_POLLY_NET_TCP_SERVER_H_
