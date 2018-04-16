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

//int main(int argc, char *argv[]) {
//  ::testing::InitGoogleTest(&argc, argv);
//  int ret = RUN_ALL_TESTS();
//  return ret;
//}

// one thread can only have one event loop
TEST(EventLoop, Duplicate) {
  EventLoop l1;
  try {
    EventLoop l2;
    FAIL() << "Fail: should not be here";
  } catch (runtime_error e) {
    // runtime error
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }
}

// loop can only be invoked in current thread
TEST(EventLoop, CurrentThread) {
  EventLoop l;
  auto res = std::async(std::launch::async, [&]() { l.loop(); });
  try {
    res.get();
    FAIL() << "Fail: should not be here";
  } catch (runtime_error e) {
    // runtime error
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }
}

TEST(EventLoop, SimpleTimer) {
  EventLoop loop;
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

  Channel channel(&loop, timerfd);
  channel.SetReadCallback([&]() {
    loop.quit();
    printf("Timeout!\n");
  });
  channel.EnableReading();

  // one shot timer
  struct itimerspec howlong;
  bzero(&howlong, sizeof howlong);
  howlong.it_value.tv_sec = 2;

  ::timerfd_settime(timerfd, 0, &howlong, nullptr);

  loop.loop();
}

TEST(EventLoop, TimerQueue) {
  EventLoop loop;
  setLogLevel(LogLevel::TRACE);

  loop.RunAfter(1, []() { printf("one shot 1s timeout\n"); });
  loop.RunEvery(1, []() { printf("1s timeout\n"); });
  loop.RunEvery(2, []() { printf("2s timeout\n"); });
  loop.RunEvery(0.3, []() { printf("0.3s timeout\n"); });

  // after 5s, quit
  loop.RunAt(Timestamp::now() + 5, [&]() {
    printf("5s timeout\n");
    loop.quit();
  });

  loop.loop();
}

TEST(EventLoopThread, Veryfication) {
  using InitialCalback = std::function<void(EventLoop *)>;

  auto func = InitialCalback();
  if (func) {
    std::cerr << "OK" << '\n';
  } else {
    std::cerr << "Not OK" << '\n';
  }
}

TEST(EventLoopThread, Basic) {
  std::function<void(EventLoop *)> callback = [](EventLoop *loop) {
    if (loop != nullptr)
      loop->RunEvery(1, []() { printf("1s timeout\n"); });
  };

  EventLoopThread thread(callback);
  thread.Start();

  ::sleep(5);
}

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
  loop.loop();
}
