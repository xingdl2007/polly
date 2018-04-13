# Copyright 2018 The Polly Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.

add_executable(eloop_t1 eloop_t1.cc)
add_executable(eloop_t2 eloop_t2.cc)

target_link_libraries(eloop_t1 polly)
target_link_libraries(eloop_t2 polly)