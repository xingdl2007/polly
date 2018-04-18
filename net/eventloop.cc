// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <assert.h>
#include <sys/poll.h>
#include <stdexcept>
#include <sys/eventfd.h>
#include "eventloop.h"
#include "poller.h"
#include "channel.h"
#include "log/logger.h"
#include "util/timestamp.h"

namespace polly {

int createEventfd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    LOG_ERROR << "Failed in eventfd";
    abort();
  }
  return evtfd;
}

thread_local EventLoop *EventLoop::t_loopInThisThread = nullptr;

EventLoop::EventLoop() : looping_(false), quit_(false),
                         callingPendingFunctors_(false),
                         threadId_(this_thread::tid()),
                         poller(std::make_unique<Poller>(this)),
                         timers_(this), wakeup_fd_(createEventfd()),
                         wakeup_channel_(this, wakeup_fd_) {
  LOG_TRACE << "EVentLoop created in thread " << threadId_;
  if (t_loopInThisThread) {
    LOG_FATAL << "Another EventLoop already exists in this thread ";
    throw std::runtime_error("EventLoop(): another EventLoop already"
                             "exists in this thread");
  } else {
    t_loopInThisThread = this;
  }

  // for wake up
  wakeup_channel_.SetReadCallback([this]() {
    this->handleRead();
  });
  wakeup_channel_.EnableReading();
}

EventLoop::~EventLoop() {
  assert(!looping_);
  t_loopInThisThread = nullptr;
  ::close(wakeup_fd_);
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
      LOG_TRACE << "EventLoop::loop(), channel " << it->fd() << " have event";
      it->HandleEvent();
    }
    LOG_TRACE << "EventLoop Poll return @" << now.toFormatedString(false);

    // start processing pending works
    doPendingFunctors();
  }
  LOG_TRACE << "EventLoop stop looping";
  looping_ = false;
}

// update: register/delete
void EventLoop::update(Channel *channel) {
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  poller->UpdateChannel(channel);
}

// for waking up from blocking poll
void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = ::write(wakeup_fd_, &one, sizeof one);
  if (n != sizeof one) {
    LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
  }
}

// level trigger, must read
void EventLoop::handleRead() {
  uint64_t one = 1;
  ssize_t n = ::read(wakeup_fd_, &one, sizeof one);
  if (n != sizeof one) {
    LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
  }
}

// just throw
void EventLoop::abortNotInLoopThread() {
  throw std::runtime_error("abortNotInLoopThread()");
}

void EventLoop::RunAt(const Timestamp &ts, const TimeCallback &cb) {
  timers_.AddTimer(cb, ts, 0);
}

void EventLoop::RunAfter(const double delay, const TimeCallback &cb) {
  timers_.AddTimer(cb, Timestamp::now() + delay, 0);
}

void EventLoop::RunEvery(const double interval, const TimeCallback &cb) {
  timers_.AddTimer(cb, Timestamp::now() + interval, interval);
}

void EventLoop::RunInLoop(const Functor &cb) {
  if (isInLoopThread()) {
    cb();
  } else {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      functors_.push_back(cb);
      LOG_TRACE << "EventLoop::RunInLoop() add a new functor";
    }
    wakeup();
  }
}

void EventLoop::doPendingFunctors() {
  std::vector<Functor> works;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    works.swap(functors_);
  }
  for (auto &w: works) {
    w();
  }
}

} // namespace polly
