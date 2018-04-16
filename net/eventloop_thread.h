// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_NET_EVENTLOOP_THREAD_H_
#define NETWORK_POLLY_NET_EVENTLOOP_THREAD_H_

#include <thread>
#include <functional>

namespace polly {

class EventLoop;

class EventLoopThread {
  using InitialCalback = std::function<void(EventLoop *)>;

public:
  EventLoopThread(InitialCalback const &cb = InitialCalback()) : callback_(cb) {}

  EventLoopThread(EventLoopThread const &) = delete;
  EventLoopThread &operator=(EventLoopThread const &) = delete;

  ~EventLoopThread();

  void Start();

private:
  std::mutex mutex_;
  EventLoop *loop_;
  InitialCalback callback_;
  std::thread thread_;
};

} // namespace polly

#endif //NETWORK_POLLY_NET_EVENTLOOP_THREAD_H_
