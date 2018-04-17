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
      next_conn_id_(0) {}

// use acceptor to listening
void TcpServer::Start() {
  acceptor_->SetNewConnectionCallback([this](int connfd,
                                             const InetAddress &remote) {

    LOG_TRACE << "TcpServer: new connection " << connfd;

    // assign every connection a name
    std::string conn_name = name_ + "-client " + std::to_string(next_conn_id_);
    next_conn_id_++;

    auto conn = std::make_shared<TcpConnection>(loop_, conn_name,
                                                connfd, remote);
    connections_[conn_name] = conn;
    conn->SetConnectionCallback(conn_callback_);
    conn->SetMessageCallback(msg_callback_);
    conn->ConnectEstablished();
  });

  // start listening
  acceptor_->listen();
}

TcpServer::~TcpServer() = default;

} //namespace polly