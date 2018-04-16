// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_NET_CHANNEL_H_
#define NETWORK_POLLY_NET_CHANNEL_H_

#include <functional>

namespace polly {

class EventLoop;

class Channel {
  typedef std::function<void()> EventCallBack;

public:
  Channel(EventLoop *loop, int fd);

  Channel(Channel const &) = delete;
  Channel &operator=(Channel const &) = delete;

  ~Channel() = default;

  void HandleEvent() const;

  void SetReadCallback(EventCallBack const &cb) {
    read_callback_ = cb;
  }
  void SetWriteCallback(EventCallBack const &cb) {
    write_callback_ = cb;
  }
  void SetErrorCallback(EventCallBack const &cb) {
    error_callback_ = cb;
  }

  void EnableReading() {
    events_ |= kReadEvent;
    update();
  }

  int fd() const { return fd_; }
  int events() const { return events_; }
  void revents(int revents) { revents_ = revents; }
  bool isNoneEvent() { return events_ == kNoneEvent; }

  // for Poller
  int index() { return index_; }
  void index(int idx) { index_ = idx; }

  EventLoop *ownerLoop() const { return loop_; }

private:
  void update();

  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  EventLoop *loop_;
  const int fd_;
  int events_;
  int revents_;
  int index_; // used by Poller

  EventCallBack read_callback_;
  EventCallBack write_callback_;
  EventCallBack error_callback_;
};

} // namespace polly

#endif //NETWORK_POLLY_NET_CHANNEL_H_
