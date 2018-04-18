// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <iostream>
#include "net/eventloop.h"
#include "net/inetaddress.h"
#include "net/tcp_server.h"
#include "net/tcp_connection.h"
#include "util/buffer.h"

using namespace std;
using namespace polly;

int main() {
  setLogLevel(LogLevel::INFO);
  EventLoop loop;

  InetAddress addr("127.0.0.1", 5000);
  TcpServer server(&loop, addr, "test");

  // only On connection, no message when connection is closed by client
  server.SetConnectionCallback([](const std::shared_ptr<TcpConnection> &conn) {
    if (conn->connected()) {
      cout << "onConnection(): new connection [" << conn->name() << "] from "
           << conn->remoteIP() << ":" << conn->remotePort() << '\n';
    } else {
      cout << "onConnection(): shutdown connection [" << conn->name() << "] from "
           << conn->remoteIP() << ":" << conn->remotePort() << '\n';
    }
  });

  server.SetMessageCallback([](const std::shared_ptr<TcpConnection> &conn,
                               Buffer *buffer,
                               Timestamp time) {
    std::cout << "onMessage(): received " << buffer->readableBytes()
              << " bytes from " << conn->name() << '\n';
    std::string msg(buffer->peek(), buffer->readableBytes());
    buffer->retrieveAll();

    // echo back
    conn->Send(msg);
  });

  server.Start();
  loop.loop();
}
