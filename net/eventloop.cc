// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <assert.h>
#include <sys/poll.h>
#include <stdexcept>
#include "eventloop.h"
#include "util/current_thread.h"

namespace polly {

thread_local EventLoop *EventLoop::t_loopInThisThread = nullptr;

EventLoop::EventLoop() : looping(false), threadId_(this_thread::gettid()) {
  // LOG_TRACE
  if (t_loopInThisThread) {
    // LOG_FATAL
    throw std::runtime_error("EventLoop(): another EventLoop already"
                             "exists in this thread");
  } else {
    t_loopInThisThread = this;
  }
}

EventLoop::~EventLoop() {
  assert(!looping);
  t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
  assert(!looping);
  assertInLoopThread();
  looping = true;

  ::poll(nullptr, 0, 5 * 1000);
  looping = false;
}

void EventLoop::abortNotInLoopThread() {
  throw std::runtime_error("abortNotInLoopThread()");
}

} // namespace polly