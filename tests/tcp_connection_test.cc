// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <thread>
#include <future>
#include <sys/timerfd.h>
#include "gtest/gtest.h"
#include "net/acceptor.h"
#include "net/eventloop.h"
#include "net/eventloop_thread.h"
#include "net/channel.h"
#include "net/tcp_server.h"
#include "net/tcp_connection.h"
#include "log/logger.h"

using namespace std;
using namespace polly;

TEST(TcpConnection, Echo) {
  setLogLevel(LogLevel::TRACE);
  EventLoop loop;

  InetAddress addr("127.0.0.1", 5000);
  TcpServer server(&loop, addr, "test");

  // only On connection, no message when connection is closed by client
  server.SetConnectionCallback([](const std::shared_ptr<TcpConnection> &conn) {
    cerr << "onConnection(): new connection [" << conn->name() << "] from "
         << conn->remoteIP() << " : " << conn->remotePort() << '\n';
  });

  server.SetMessageCallback([](const std::shared_ptr<TcpConnection> &conn,
                               Buffer *buffer,
                               Timestamp time) {
    std::cerr << "onMessage(): received " << buffer->readableBytes()
              << " bytes from " << conn->name() << '\n';

    std::string msg(buffer->peek(), buffer->readableBytes());
    buffer->retrieveAll();

    conn->Send(msg);
  });

  server.Start();
  loop.loop();
}