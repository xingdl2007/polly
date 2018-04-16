// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_UTIL_TIMESTAMP_H_
#define NETWORK_POLLY_UTIL_TIMESTAMP_H_

#include <string>

namespace polly {

class Timestamp {
  friend bool operator==(Timestamp const &lhs, Timestamp const &rhs);
  friend bool operator<(Timestamp const &lhs, Timestamp const &rhs);

public:
  Timestamp(int64_t micro = 0) : micro_since_epoch_(micro) {}

  std::string toString() const;
  std::string toFormatedString(bool showMicroseconds) const;

  static const int kMicroSecondsPerSecond = 1000 * 1000;

  static Timestamp now();

  Timestamp operator+(double seconds) {
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(micro_since_epoch_ + delta);
  }

private:
  int64_t micro_since_epoch_;  // microseconds since epoch
};

inline bool operator==(Timestamp const &lhs, Timestamp const &rhs) {
  return lhs.micro_since_epoch_ == rhs.micro_since_epoch_;
}

inline bool operator!=(Timestamp const &lhs, Timestamp const &rhs) {
  return !(lhs == rhs);
}

inline bool operator<(Timestamp const &lhs, Timestamp const &rhs) {
  return lhs.micro_since_epoch_ < rhs.micro_since_epoch_;
}

}// namespace polly

#endif //NETWORK_POLLY_UTIL_TIMESTAMP_H_
