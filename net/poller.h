// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_NET_POLLER_H_
#define NETWORK_POLLY_NET_POLLER_H_

#include <vector>
#include <map>
#include "eventloop.h"

struct pollfd;

namespace polly {

class Channel;
class Timestamp;

// IO Multiplexing with poll(2)
//
// This class does not own the Channel objects.
class Poller {
  typedef std::vector<Channel *> ChannelList;

public:
  Poller(EventLoop *loop) : ownerLoop_(loop) {}

  Poller(Poller const &) = delete;
  Poller &operator=(Poller const &) = delete;

  ~Poller() = default;

  // Polls the I/O events;
  Timestamp Poll(int timeout, ChannelList *active);

  // Change the interested I/O events, must be called in the loop thread;
  void UpdateChannel(Channel *channel);

private:
  void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }
  void fillActiveChannels(int numEvents, ChannelList *active) const;

  typedef std::vector<struct pollfd> PollFdList;
  typedef std::map<int, Channel *> ChannelMap;

  EventLoop *ownerLoop_;
  PollFdList pollfds_;
  ChannelMap channels_;
};

} // namespace polly

#endif //NETWORK_POLLY_NET_POLLER_H_
