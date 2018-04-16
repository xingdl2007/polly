// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_NET_EVENTLOOP_H_
#define NETWORK_POLLY_NET_EVENTLOOP_H_

#include <unistd.h>
#include <memory>
#include <vector>
#include <functional>
#include "util/process_info.h"
#include "timer_queue.h"

namespace polly {

class Poller;
class Channel;
class Timestamp;

class EventLoop {
  using TimeCallback = std::function<void()>;
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
    return threadId_ == this_thread::tid();
  }

  void update(Channel *);

  void quit() { quit_ = true; }

  // timer related
  void RunAt(const Timestamp &ts, const TimeCallback &cb);
  void RunAfter(const double delay, const TimeCallback &cb);
  void RunEvery(double interval, const TimeCallback &cb);

private:
  typedef std::vector<Channel *> ChannelList;
  void abortNotInLoopThread();

  bool looping_;
  bool quit_;
  const pid_t threadId_;

  std::unique_ptr<Poller> poller;
  ChannelList active_channels_;

  TimerQueue timers_;

  // record for IO thread, which construct `this` EventLoop
  thread_local static EventLoop *t_loopInThisThread;
};

} // namespace polly

#endif //NETWORK_POLLY_NET_EVENTLOOP_H_
