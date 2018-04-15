// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "gtest/gtest.h"
#include "log/logger.h"
#include "log/async_logging.h"

using namespace std;
using namespace polly;

TEST(Logging, Basic) {
  setLogLevel(LogLevel::TRACE);

  LOG_TRACE << "h";
  LOG_DEBUG << "e";
  LOG_INFO << "l";
  LOG_WARNING << "l";
  LOG_ERROR << "o";
  LOG_FATAL << "!";
}

TEST(LogFile, Basic) {
  LogFile log_file("logfile");
  log_file.Append(Slice("hello\n"));
}

TEST(AsyncLogging, Basic) {
  setLogLevel(LogLevel::TRACE);
  AsyncLogging async("async-basic");

  Logger::destination([&](Slice const &slice) {
    async.Append(slice);
  });

  async.StartWorker();

  LOG_TRACE << "h";
  LOG_DEBUG << "e";
  LOG_INFO << "l";
  LOG_WARNING << "l";
  LOG_ERROR << "o";
  LOG_FATAL << "!";

  // make sure worker thread is waiting
  ::sleep(1);
  async.StopWorker();
}

TEST(AsyncLogging, Basic2) {
  setLogLevel(LogLevel::TRACE);
  AsyncLogging async("async-basic2");

  Logger::destination([&](Slice const &slice) {
    async.Append(slice);
  });

  async.StartWorker();

  Slice slice("123456789 hello world ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz ");

  auto start = std::chrono::system_clock::now();
  int count = 5000000;
  for (int i = 0; i < count; ++i) {
    LOG_INFO << slice << i + 1;
  }
  std::chrono::nanoseconds elapse = std::chrono::system_clock::now() - start;
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapse).count();
  auto data = slice.size() * count * 1.0 / 1024 / 1024;
  std::cerr << seconds << " s, " << data / seconds << " MiB/s\n";

  // make sure worker thread is waiting
  ::sleep(1);
  async.StopWorker();
}

TEST(AsyncLogging, MultiThread) {
  setLogLevel(LogLevel::TRACE);
  AsyncLogging async("async-basic2");

  Logger::destination([&](Slice const &slice) {
    async.Append(slice);
  });

  async.StartWorker();

  Slice slice("123456789 hello world ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz ");

  int count = 5000000;

  std::promise<void> go, t1, t2;
  std::shared_future<void> ready = go.get_future();

  auto func1 = [&, ready]() {
    t1.set_value();
    ready.wait();
    for (int i = 0; i < count / 2; ++i) {
      LOG_INFO << slice << i + 1;
    }
  };
  auto func2 = [&, ready]() {
    t2.set_value();
    ready.wait();
    for (int i = 0; i < count / 2; ++i) {
      LOG_INFO << slice << i + 1;
    }
  };

  std::thread thread1(func1);
  std::thread thread2(func2);

  // ready?
  t1.get_future().wait();
  t2.get_future().wait();

  // go!
  auto start = std::chrono::system_clock::now();
  go.set_value();
  thread1.join();
  thread2.join();

  std::chrono::nanoseconds elapse = std::chrono::system_clock::now() - start;
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapse).count();
  auto data = slice.size() * count * 1.0 / 1024 / 1024;
  std::cerr << seconds << " s, " << data / seconds << " MiB/s\n";

  // make sure worker thread is waiting
  ::sleep(1);
  async.StopWorker();
}