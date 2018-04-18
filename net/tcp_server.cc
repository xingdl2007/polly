// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "tcp_server.h"
#include "eventloop.h"
#include "acceptor.h"
#include "tcp_connection.h"

namespace polly {

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listen_addr,
                     std::string name)
    : loop_(loop), name_(name),
      acceptor_(std::make_unique<Acceptor>(loop, listen_addr)),
      next_conn_id_(0), listen_addr_(listen_addr) {}

// TcpServer will keep TcpConnection
TcpServer::~TcpServer() {
  loop_->assertInLoopThread();
  LOG_TRACE << "TcpServer::~TcpServer " << name_ << " destructing";

  for (auto it(connections_.begin()); it != connections_.end(); ++it) {
    std::shared_ptr<TcpConnection> conn(it->second);
    it->second.reset();

    conn->getLoop()->RunInLoop(
        std::bind(&TcpConnection::ConnectDestroyed, conn));
  }
}

// use acceptor to listen
void TcpServer::Start() {
  acceptor_->SetNewConnectionCallback([this](int connfd,
                                             const InetAddress &remote) {
    loop_->assertInLoopThread();

    // assign every connection a name
    std::string conn_name = name_ + "#" + std::to_string(next_conn_id_);
    next_conn_id_++;

    LOG_TRACE << "TcpServer: new connection " << conn_name
              << " from " << remote.IP() << ":" << remote.Port();

    auto conn = std::make_shared<TcpConnection>(loop_, conn_name, connfd,
                                                listen_addr_, remote);
    connections_[conn_name] = conn;
    conn->SetConnectionCallback(conn_callback_);
    conn->SetMessageCallback(msg_callback_);

    conn->SetCloseCallback([this](const std::shared_ptr<TcpConnection> &c) {
      loop_->assertInLoopThread();
      LOG_INFO << "TcpServer::removeConnection()";

      size_t n = connections_.erase(c->name());
      assert(n == 1);

      // keep TcpConnection object until TcpServer is destructing
      loop_->RunInLoop(
          std::bind(&TcpConnection::ConnectDestroyed, c));
    });

    // finally
    conn->ConnectEstablished();
  });

  // start listening
  acceptor_->listen();
}

} //namespace polly