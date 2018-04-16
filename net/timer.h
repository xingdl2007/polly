// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_NET_TIMER_H_
#define NETWORK_POLLY_NET_TIMER_H_

#include <functional>
#include "util/timestamp.h"

namespace polly {

class Timer {
  using TimerCallBack = std::function<void()>;

public:
  Timer(const TimerCallBack &cb, Timestamp ts, double interval)
      : callback_(cb), expiration_(ts), repeat_(interval > 0.0), interval_(interval) {}

  Timer(Timer const &) = delete;
  Timer &operator=(Timer const &) = delete;

  ~Timer() = default;

  void Fire() const {
    callback_();
  }

  void ReArm(Timestamp now) {
    if (repeat_) {
      expiration_ = now + interval_;
    } else {
      expiration_ = Timestamp();
    }
  }

private:
  const TimerCallBack callback_;
  Timestamp expiration_;
  const bool repeat_;
  const double interval_; // seconds
};

} // namespace polly

#endif //NETWORK_POLLY_NET_TIMER_H_
