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
#include "log/logger.h"

using namespace std;
using namespace polly;

TEST(Acceptor, Basic) {
  EventLoop loop;

  InetAddress server("192.168.0.108", 5000);
  Acceptor acceptor(&loop, server);

// register new connection callback
  acceptor.SetNewConnectionCallback([](int connfd, const InetAddress &client) {
    printf("client: %s, port: %d\n", client.IP(), client.Port());
    ::write(connfd, "How are you?\n", 13);
    ::close(connfd);
  });

  acceptor.listen();
  loop.loop();
}