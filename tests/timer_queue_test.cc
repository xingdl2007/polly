// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <iostream>
#include "gtest/gtest.h"
#include "net/eventloop.h"
#include "util/timer.h"
#include "net/timer_queue.h"

using namespace std;
using namespace polly;

TEST(Timer, Basic) {
  EventLoop loop;
  TimerQueue queue(&loop);

  // after 1s from now
  queue.AddTimer([&]() {
    printf("1s timeout!\n");
    loop.quit();
  }, Timestamp::now() + 1, 1);

  loop.loop();
}

TEST(TimerQueue, Basic) {
  EventLoop loop;
  TimerQueue queue(&loop);

  // after 1s from now
  queue.AddTimer([]() {
    printf("1s timeout!\n");
  }, Timestamp::now() + 1, 1);

  // after 2s from now
  queue.AddTimer([]() {
    printf("2s timeout!\n");
  }, Timestamp::now() + 2, 2);

  queue.AddTimer([]() {
    printf("0.5s timeout!\n");
  }, Timestamp::now() + 0.5, 0.5);

  queue.AddTimer([&]() {
    printf("quit after 5s!\n");
    loop.quit();
  }, Timestamp::now() + 5, 0);

  loop.loop();
}
