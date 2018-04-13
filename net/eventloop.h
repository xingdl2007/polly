// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_NET_EVENTLOOP_H_
#define NETWORK_POLLY_NET_EVENTLOOP_H_

#include <unistd.h>
#include "util/current_thread.h"

namespace polly {

class EventLoop {
public:
  EventLoop();
  ~EventLoop();

  // disable copy
  EventLoop(EventLoop const &) = delete;
  EventLoop &operator=(EventLoop const &) = delete;

  // will block in invoking thread
  void loop();

  void assertInLoopThread() {
    if (!isInLoopThread()) {
      abortNotInLoopThread();
    }
  }

  bool isInLoopThread() const {
    return threadId_ == this_thread::gettid();
  }

private:
  void abortNotInLoopThread();

  bool looping;
  const pid_t threadId_;

  // record for IO thread, which construct `this` EventLoop
  thread_local static EventLoop *t_loopInThisThread;
};

} // namespace polly

#endif //NETWORK_POLLY_NET_EVENTLOOP_H_
