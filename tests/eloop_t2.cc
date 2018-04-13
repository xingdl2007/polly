// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <thread>
#include "net/eventloop.h"

using namespace polly;

// event loop can only loop in current thread

int main() {
  EventLoop l;
  std::thread t([&]() { l.loop(); });
  t.join();
  return 0;
}