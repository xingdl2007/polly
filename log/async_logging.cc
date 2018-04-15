// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <functional>
#include "async_logging.h"

namespace polly {

AsyncLogging::AsyncLogging(std::string const &file)
    : cur_buffer_(new Buffer), next_buffer_(new Buffer),
      thread_(nullptr), done(false), log_file_(file) {}

// thread safe
void AsyncLogging::Append(Slice const &slice) {
  std::lock_guard<std::mutex> lock(mutex_);

  // find valid buffer
  if (cur_buffer_->space() >= slice.size()) {
    cur_buffer_->append(slice);
  } else {
    buffers_.push_back(std::move(cur_buffer_));

    if (next_buffer_) {
      cur_buffer_ = std::move(next_buffer_);
    } else {
      cur_buffer_ = std::make_unique<Buffer>();
    }
    cur_buffer_->append(slice);
    cond_.notify_one();
  }
}

void AsyncLogging::ThreadFunction(std::promise<void> &promise) {
  // sync with `StartWorker()`
  promise.set_value();

  // swap buffers
  auto tempBuffer1 = std::make_unique<Buffer>();
  auto tempBuffer2 = std::make_unique<Buffer>();
  std::vector<std::unique_ptr<Buffer>> tempBuffers;

  while (!done) {
    assert(tempBuffer1 != nullptr);
    assert(tempBuffer2 != nullptr);
    assert(tempBuffers.empty());
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (buffers_.empty()) {
        cond_.wait_for(lock, std::chrono::seconds(3));
      }
      tempBuffer1.swap(cur_buffer_);
      if (next_buffer_ == nullptr) {
        tempBuffer2.swap(next_buffer_);
      }
      tempBuffers.swap(buffers_);
    }

    tempBuffers.push_back(std::move(tempBuffer1));
    if (tempBuffer2 != nullptr) {
      tempBuffers.push_back(std::move(tempBuffer2));
    }
    for (auto &it: tempBuffers) {
      log_file_.Append(it->slice());
    }

    // tempBuffers at least have two buffers
    tempBuffers.resize(2);
    tempBuffer1 = std::move(tempBuffers.back());
    tempBuffers.pop_back();
    tempBuffer2 = std::move(tempBuffers.back());
    tempBuffers.pop_back();
    tempBuffers.clear();

    tempBuffer1->reset();
    tempBuffer2->reset();
  }
}

// start distinct thread, write logs to disk file.
void AsyncLogging::StartWorker() {
  std::promise<void> promise;
  std::future<void> future = promise.get_future();

  // method 1: use bind/function
  // thread_ = std::make_unique<std::thread>(
  //    std::bind(&AsyncLogging::ThreadFunction, this,
  //              std::placeholders::_1),
  //    std::ref(promise));

  // method 2 : use lambda
  thread_ = std::make_unique<std::thread>([&]() {
    this->ThreadFunction(promise);
  });

  // make sure thread is running
  future.wait();
}

// wait worker thread exit
void AsyncLogging::StopWorker() {
  done = false;
  cond_.notify_one();
  thread_->join();
}

} // namespace polly