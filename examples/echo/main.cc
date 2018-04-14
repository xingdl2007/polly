// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <thread>
#include <iostream>
#include "net/eventloop.h"

int main() {
  std::thread t;
  if (t.joinable()) {
    std::cout << "joinable" << '\n';
  }
  return 0;
}
