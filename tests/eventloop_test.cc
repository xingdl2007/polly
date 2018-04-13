// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <thread>
#include <future>
#include "gtest/gtest.h"
#include "net/eventloop.h"

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
  } catch (runtime_error e) {
    // runtime error
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }
}

TEST(EventLoop, Basic) {
  EventLoop l;
  l.loop();
}