# Copyright 2018 The Polly Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.

cmake_minimum_required(VERSION 3.5)
project(polly VERSION 0.1.0 LANGUAGES C CXX)

# This project can use C11, but will gracefully decay down to C89.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)

# This project requires C++14.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# default debug build
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")  # Debug or Release
endif ()

set(CMAKE_CXX_COMPILER "g++")

# general build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")

# for debug build
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")

# for release build
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -finline-limit=1000 -DNDEBUG")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(
        "${PROJECT_SOURCE_DIR}"
        "${PROJECT_SOURCE_DIR}/include")

# expose only necessary interface in the future
set(POLLY_PUBLIC_INCLUDE_DIR "include/polly")

add_library(polly "")

# pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(polly Threads::Threads)

include(gtest.cmake)

# all public for now
target_sources(polly PUBLIC
        "${PROJECT_SOURCE_DIR}/net/channel.cc"
        "${PROJECT_SOURCE_DIR}/net/channel.h"
        "${PROJECT_SOURCE_DIR}/net/eventloop.cc"
        "${PROJECT_SOURCE_DIR}/net/eventloop.h"
        "${PROJECT_SOURCE_DIR}/net/poller.cc"
        "${PROJECT_SOURCE_DIR}/net/poller.h"
        "${PROJECT_SOURCE_DIR}/net/timer.h"
        "${PROJECT_SOURCE_DIR}/net/timer_queue.cc"
        "${PROJECT_SOURCE_DIR}/net/timer_queue.h"
        "${PROJECT_SOURCE_DIR}/log/async_logging.cc"
        "${PROJECT_SOURCE_DIR}/log/async_logging.h"
        "${PROJECT_SOURCE_DIR}/log/file.cc"
        "${PROJECT_SOURCE_DIR}/log/file.h"
        "${PROJECT_SOURCE_DIR}/log/log_file.cc"
        "${PROJECT_SOURCE_DIR}/log/log_file.h"
        "${PROJECT_SOURCE_DIR}/log/log_stream.cc"
        "${PROJECT_SOURCE_DIR}/log/log_stream.h"
        "${PROJECT_SOURCE_DIR}/log/logger.cc"
        "${PROJECT_SOURCE_DIR}/log/logger.h"
        "${PROJECT_SOURCE_DIR}/util/buffer.cc"
        "${PROJECT_SOURCE_DIR}/util/buffer.h"
        "${PROJECT_SOURCE_DIR}/util/copyable.h"
        "${PROJECT_SOURCE_DIR}/util/process_info.cc"
        "${PROJECT_SOURCE_DIR}/util/process_info.h"
        "${PROJECT_SOURCE_DIR}/util/timestamp.cc"
        "${PROJECT_SOURCE_DIR}/util/timestamp.h"
        "${PROJECT_SOURCE_DIR}/include/polly/slice.h")

# tests
add_subdirectory(tests)

# examples
add_subdirectory(examples)
