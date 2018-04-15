// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_UTIL_TIMESTAMP_H_
#define NETWORK_POLLY_UTIL_TIMESTAMP_H_

#include <string>

namespace polly {

class TimeStamp {
public:
  TimeStamp(int64_t micro = 0) : micro_since_epoch_(micro) {}

  std::string toString() const;
  std::string toFormatedString(bool showMicroseconds) const;

  static const int kMicroSecondsPerSecond = 1000 * 1000;

  static TimeStamp now();
private:
  int64_t micro_since_epoch_;  // microseconds since epoch
};

} // namespace polly

#endif //NETWORK_POLLY_UTIL_TIMESTAMP_H_
