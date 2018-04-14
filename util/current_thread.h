// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_UTIL_CURRENT_THREAD_H_
#define NETWORK_POLLY_UTIL_CURRENT_THREAD_H_

#include <unistd.h>

namespace polly {
namespace this_thread {

pid_t tid();

} // namespace this_thread
} // namespace polly

#endif // NETWORK_POLLY_UTIL_CURRENT_THREAD_H_
