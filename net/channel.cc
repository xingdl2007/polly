// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <sys/poll.h>
#include "channel.h"
#include "eventloop.h"
#include "log/logger.h"

namespace polly {

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd),
                                            events_(0), revents_(0),
                                            index_(-1) {}

void Channel::HandleEvent() const {
  if (revents_ & POLLHUP) {
    LOG_WARNING << "Channel::HandleEvent() POLLHUP";
    if (close_callback_) {
      close_callback_();
    }
  }
  if (revents_ & POLLNVAL) {
    LOG_WARNING << "Channel::HandleEvent() POLLNVAL";
  }
  if (revents_ & (POLLERR | POLLNVAL)) {
    if (error_callback_) {
      error_callback_();
    }
  }
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (read_callback_) {
      read_callback_();
    }
  }
  if (revents_ & POLLOUT) {
    if (write_callback_) {
      write_callback_();
    }
  }
}

void Channel::update() {
  loop_->update(this);
}

} // namespace polly
