// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef POLLY_CURRENT_THREAD_H
#define POLLY_CURRENT_THREAD_H

#include <unistd.h>

namespace polly {
namespace this_thread {

pid_t gettid();

} // namespace this_thread
} // namespace polly

#endif //POLLY_CURRENT_THREAD_H
