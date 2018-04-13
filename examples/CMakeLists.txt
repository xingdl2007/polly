# Copyright 2018 The Polly Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.

add_executable(e_echo echo/main.cc)
target_link_libraries(e_echo polly)

# weak callback
add_executable(weakcallback callback/weak.cc)
target_link_libraries(weakcallback libgtest libgtest_main)