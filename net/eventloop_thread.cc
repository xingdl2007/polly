// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <future>
#include "eventloop.h"
#include "eventloop_thread.h"

namespace polly {

EventLoop *EventLoopThread::Start() {
  std::promise<void> promise;
  thread_ = std::thread([&]() {
    EventLoop loop;
    loop_ = &loop;
    
    promise.set_value();
    if (callback_) {
      callback_(&loop);
    }
    loop.loop();
  });

  promise.get_future().wait();
  return loop_;
}

EventLoopThread::~EventLoopThread() {
  if (loop_) {
    loop_->quit();
  }
  if (thread_.joinable()) {
    thread_.join();
  }
}

} // namespace polly