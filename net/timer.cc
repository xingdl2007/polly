// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "timer.h"

namespace polly {

std::atomic<int64_t> Timer::CreatedNum_{0};

Timer::Timer(const TimerCallBack &cb, Timestamp ts, double interval)
    : callback_(cb), expiration_(ts), repeat_(interval > 0.0),
      interval_(interval), seq_num_(CreatedNum_++) {}

} // namespace polly