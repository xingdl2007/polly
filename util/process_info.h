// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_UTIL_PROCESS_INFO_H_
#define NETWORK_POLLY_UTIL_PROCESS_INFO_H_

#include <string>

namespace polly {
namespace this_thread {

pid_t tid();

} // namespace this_thread

namespace process {

pid_t pid();
std::string pidString();

std::string hostname();

} // namespace process
} // namespace polly

#endif //NETWORK_POLLY_UTIL_PROCESS_INFO_H_
