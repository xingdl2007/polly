// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_NET_TIMER_QUEUE_H_
#define NETWORK_POLLY_NET_TIMER_QUEUE_H_

#include <map>
#include <memory>
#include <vector>
#include <search.h>
#include "channel.h"
#include "util/timestamp.h"

namespace polly {

class EventLoop;
class Timer;

// A best efforts timer queue
// No guarantee that the callback will be on time
class TimerQueue {
  using TimerCallback = std::function<void()>;
  using TimerTable = std::multimap<Timestamp, std::unique_ptr<Timer>>;
  using Iterator = TimerTable::iterator;

public:
  explicit TimerQueue(EventLoop *loop);

  TimerQueue(TimerQueue const &) = delete;
  TimerQueue &operator=(TimerQueue const &) = delete;

  ~TimerQueue();

  // Schedules the callback to be run at given time,
  // repeats if interval > 0.0
  //
  // Must be thread safe. Usually be called from other threads.
  void AddTimer(const TimerCallback &cb, Timestamp when, double interval);

  // called when timerfd alarms
  void HandleRead();

  // move out all expired timers
  TimerTable GetExpired(Timestamp now);

private:
  void reset(TimerTable &expired, Timestamp now);

  EventLoop *loop_;
  const int timerfd_;
  Channel timerfd_channel_;

  // ordered by expiration time, so timer resolution is microsecond
  // if @Timestamp already have a timer, add will fail
  TimerTable timers_;
};

} // namespace polly

#endif //NETWORK_POLLY_NET_TIMER_QUEUE_H_
