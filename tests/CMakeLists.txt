# Copyright 2018 The Polly Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.

# eventloop
add_executable(t_eventloop eventloop_test.cc)
target_link_libraries(t_eventloop polly libgtest libgtest_main)

# logger
add_executable(t_logging logging_test.cc)
target_link_libraries(t_logging polly libgtest libgtest_main)

# timer/timer_queue
add_executable(t_timer timer_queue_test.cc)
target_link_libraries(t_timer polly libgtest libgtest_main)

# acceptor
add_executable(t_acceptor acceptor_test.cc)
target_link_libraries(t_acceptor polly libgtest libgtest_main)

# tcp server
add_executable(t_tcp_server tcp_server_test.cc)
target_link_libraries(t_tcp_server polly libgtest libgtest_main)

# tcp connection
add_executable(t_conn tcp_connection_test.cc)
target_link_libraries(t_conn polly libgtest libgtest_main)
