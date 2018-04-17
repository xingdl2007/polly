// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <assert.h>
#include <sys/poll.h>
#include <stdexcept>
#include "eventloop.h"
#include "poller.h"
#include "channel.h"
#include "log/logger.h"
#include "util/timestamp.h"

namespace polly {

thread_local EventLoop *EventLoop::t_loopInThisThread = nullptr;

EventLoop::EventLoop() : looping_(false), quit_(false),
                         threadId_(this_thread::tid()),
                         poller(std::make_unique<Poller>(this)),
                         timers_(this) {
  LOG_TRACE << "EVentLoop created in thread " << threadId_;
  if (t_loopInThisThread) {
    LOG_FATAL << "Another EventLoop already exists in this thread ";
    throw std::runtime_error("EventLoop(): another EventLoop already"
                             "exists in this thread");
  } else {
    t_loopInThisThread = this;
  }
}

EventLoop::~EventLoop() {
  assert(!looping_);
  t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
  assert(!looping_);
  assertInLoopThread();
  looping_ = true;
  quit_ = false;

  while (!quit_) {
    active_channels_.clear();

    // -1: wait until some file descriptor ready
    Timestamp now = poller->Poll(-1, &active_channels_);
    for (auto const it: active_channels_) {
      LOG_INFO << "EventLoop::loop(), channel " << it->fd() << " have event";
      it->HandleEvent();
    }
    LOG_TRACE << "EventLoop Poll return @" << now.toFormatedString(false);
  }
  LOG_TRACE << "EventLoop stop looping";
  looping_ = false;
}

void EventLoop::update(Channel *channel) {
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller->UpdateChannel(channel);
}

void EventLoop::abortNotInLoopThread() {
  throw std::runtime_error("abortNotInLoopThread()");
}

void EventLoop::RunAt(const Timestamp &ts, const TimeCallback &cb) {
  timers_.AddTimer(cb, ts, 0);
}

void EventLoop::RunAfter(const double delay, const TimeCallback &cb) {
  timers_.AddTimer(cb, Timestamp::now() + delay, 0);
}

void EventLoop::RunEvery(double interval, const TimeCallback &cb) {
  timers_.AddTimer(cb, Timestamp::now() + interval, interval);
}

} // namespace polly