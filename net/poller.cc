// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <sys/poll.h>
#include "poller.h"
#include "channel.h"
#include "util/timestamp.h"
#include "log/logger.h"

namespace polly {

// Polls the I/O events;
// parameters: timeout (the number of milliseconds poll(2) should block
// waiting before a file descriptor to become ready)
TimeStamp Poller::Poll(int timeout, ChannelList *active) {
  // 1. construct struct pollfd from PollFdList
  // 2. call poll(2)
  // 3. fill active channels when poll(2) returns
  // 4. return now()
  int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeout);
  TimeStamp now = TimeStamp::now();

  if (numEvents > 0) {
    LOG_TRACE << "Poller::Poll(): " << numEvents << " events happened";
    fillActiveChannels(numEvents, active);
  } else if (numEvents == 0) {
    LOG_TRACE << "Poller::Poll(): nothing happened";
  } else {
    LOG_ERROR << "Poller::Poll(): failed.";
  }
  return now;
}

// Change the interested I/O events, must be called in the loop thread;
void Poller::UpdateChannel(Channel *channel) {
  assertInLoopThread();
  LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events();
  if (channel->index() < 0) {
    // a new one, add to pollfds_
    assert(channels_.find(channel->fd()) == channels_.end());
    struct pollfd pfd = {
        channel->fd(),
        static_cast<short>(channel->events()),
        0
    };
    pollfds_.push_back(pfd);
    channel->index(static_cast<int>(pollfds_.size() - 1));
    channels_[channel->fd()] = channel;
  } else {
    // update existing one
    assert(channels_.find(channel->fd()) != channels_.end());
    assert(channel == channels_[channel->fd()]);

    int idx = channel->index();
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));

    struct pollfd &pfd = pollfds_[idx];
    assert(pfd.fd == channel->fd() || pfd.fd == -1);

    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    if (channel->isNoneEvent()) {
      pfd.fd = -1;
    }
  }
}

void Poller::fillActiveChannels(int numEvents, ChannelList *active) const {
  for (auto const &it: pollfds_) {
    if (it.revents > 0) {
      auto ch = channels_.find(it.fd);
      assert(ch != channels_.end());

      Channel *channel = ch->second;
      assert(channel->fd() == it.fd);

      channel->revents(it.revents);
      active->push_back(channel);

      if (--numEvents == 0) {
        break;
      }
    }
  }
}

} // namespace polly
