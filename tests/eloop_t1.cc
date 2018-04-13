// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "net/eventloop.h"

using namespace polly;

// one thread can only have one event loop

int main() {
  EventLoop l1;
  EventLoop l2;
  return 0;
}