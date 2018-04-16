// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <iostream>
#include "gtest/gtest.h"
#include "net/timer.h"
#include "net/timer_queue.h"

using namespace std;
using namespace polly;

TEST(Timer, Basic) {
  std::map<int, std::unique_ptr<int>> imap;

  imap.emplace(2, std::make_unique<int>(2));
  imap.emplace(1, std::make_unique<int>(1));
  imap.emplace(3, std::make_unique<int>(3));

  for (auto it = imap.cbegin(); it != imap.cend(); ++it) {
    std::cerr << it->first << " " << *(it->second) << '\n';
  }
}

TEST(TimerQueue, Basic) {

}