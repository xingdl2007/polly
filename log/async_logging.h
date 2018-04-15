// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_LOG_ASYNC_LOGGING_H_
#define NETWORK_POLLY_LOG_ASYNC_LOGGING_H_

#include <memory>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "util/buffer.h"
#include "log_file.h"

namespace polly {

class AsyncLogging {
  typedef FixedBuffer<kSmallBufferSize> Buffer;

public:
  AsyncLogging(std::string const &file);

  AsyncLogging(AsyncLogging const &) = delete;
  AsyncLogging &operator=(AsyncLogging const &) = delete;

  ~AsyncLogging() = default;

  void Append(Slice const &);

  void ThreadFunction(std::promise<void> &);

  // start distinct thread, write logs to disk file.
  void StartWorker();

  // wait worker thread exit
  void StopWorker();

private:
  std::mutex mutex_;
  std::condition_variable cond_;
  std::unique_ptr<Buffer> cur_buffer_, next_buffer_;
  std::vector<std::unique_ptr<Buffer>> buffers_;

  std::unique_ptr<std::thread> thread_; // worker thread
  std::atomic<bool> done;

  LogFile log_file_; // underlying disk file;
};

} // namespace polly

#endif // NETWORK_POLLY_LOG_ASYNC_LOGGING_H_
