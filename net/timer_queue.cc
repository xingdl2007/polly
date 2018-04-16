// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <unistd.h>
#include <sys/timerfd.h>
#include "log/logger.h"
#include "timer.h"
#include "timer_queue.h"

namespace polly {

int createTimerfd() {
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0) {
    LOG_FATAL << "Failed in timerfd_create";
  }
  return timerfd;
}

void readTimerfd(int timerfd, Timestamp now) {
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
  LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at " << now.toString();
  if (n != sizeof howmany) {
    LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
  }
}

struct timespec howMuchTimeFromNow(Timestamp when) {
  int64_t microseconds = when.microSecondsSinceEpoch()
      - Timestamp::now().microSecondsSinceEpoch();
  if (microseconds < 100) {
    microseconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(
      microseconds / Timestamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
      (microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
  return ts;
}

void resetTimerfd(int timerfd, Timestamp expiration) {
  // wake up loop by timerfd_settime()
  struct itimerspec newValue;
  struct itimerspec oldValue;

  bzero(&newValue, sizeof newValue);
  bzero(&oldValue, sizeof oldValue);
  newValue.it_value = howMuchTimeFromNow(expiration);

  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if (ret) {
    LOG_ERROR << "timerfd_settime()";
  }
}

TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop), timerfd_(createTimerfd()),
      timerfd_channel_(loop, timerfd_) {

  timerfd_channel_.SetReadCallback([&]() {
    this->HandleRead();
  });
  timerfd_channel_.EnableReading();
}

TimerQueue::~TimerQueue() {
  ::close(timerfd_);
}

// called when timerfd alarms
void TimerQueue::HandleRead() {
  Timestamp now(Timestamp::now());
  readTimerfd(timerfd_, now);

  auto expired = GetExpired(now);
  for (auto it = expired.begin(); it != expired.end(); ++it) {
    it->second->Fire();
  }
  reset(expired, now);
}

TimerQueue::TimerTable TimerQueue::GetExpired(Timestamp now) {
  auto bound = timers_.lower_bound(now);
  TimerTable expired;
  for (auto it = timers_.begin(); it != bound; ++it) {
    expired.emplace(it->first, std::move(it->second));
  }
  timers_.erase(timers_.begin(), bound);
  return expired;
};

void TimerQueue::AddTimer(const TimerCallback &cb, Timestamp when,
                          double interval) {
  bool needUpdate = false;
  if (timers_.empty() || when < timers_.begin()->second->Expiration()) {
    needUpdate = true;
  }

  timers_.emplace(when, std::make_unique<Timer>(cb, when, interval));

  if (needUpdate) {
    Timestamp nextExpire = timers_.begin()->second->Expiration();

    if (nextExpire.valid()) {
      resetTimerfd(timerfd_, nextExpire);
    }
  }
}

void TimerQueue::reset(TimerTable &expired, Timestamp now) {
  Timestamp nextExpire;

  for (auto it = expired.begin(); it != expired.end(); ++it) {
    if (it->second->isRepeat()) {
      it->second->ReArm(now);
      Timestamp when = it->second->Expiration();
      timers_.emplace(when, std::move(it->second));
    }
  }
  if (!timers_.empty()) {
    nextExpire = timers_.begin()->second->Expiration();
  }

  if (nextExpire.valid()) {
    resetTimerfd(timerfd_, nextExpire);
  }
}

} // namespace polly
