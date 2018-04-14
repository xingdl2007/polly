// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "gtest/gtest.h"
#include "log/logging.h"

using namespace std;
using namespace polly;

TEST(Logging, Basic) {
  LOG_TRACE << "h";
  LOG_DEBUG << "e";
  LOG_INFO << "l";
  LOG_WARNING << "l";
  LOG_ERROR << "o";
  LOG_FATAL << "!\n";
}
