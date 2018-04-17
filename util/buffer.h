// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_UTIL_BUFFER_H_
#define NETWORK_POLLY_UTIL_BUFFER_H_

#include <string.h>
#include <vector>
#include <functional>
#include <include/polly/slice.h>

namespace polly {

const int kSmallBufferSize = 4096;

template<int N>
class FixedBuffer {
public:
  FixedBuffer() : cur_(data_) { bzero(); }

  FixedBuffer(FixedBuffer const &) = delete;
  FixedBuffer &operator=(FixedBuffer const &) = delete;

  ~FixedBuffer() = default;

  // data + size
  const char *data() { return data_; }
  size_t size() { return static_cast<size_t>(cur_ - data_); }

  Slice slice() { return Slice(data_, size()); }

  // available space
  size_t space() { return static_cast<size_t >(end() - cur_); }

  void bzero() { ::bzero(data_, sizeof data_); }

  void reset() { cur_ = data_; }

  // after last element pointer
  char *end() { return data_ + N; }

  void append(Slice const &s) {
    assert(space() >= s.size());
    ::memcpy(cur_, s.data(), s.size());
    cur_ += s.size();
  }

private:
  char data_[N];
  char *cur_;
};

// explicit instantiations
template class FixedBuffer<kSmallBufferSize>;

// for send/receive TCP byte stream, application level buffer
class Buffer {
public:
  Buffer(size_t initialSize = kInitialSize)
      : buffer_(kCheapPrepend + initialSize), readerIndex_(kCheapPrepend),
        writerIndex_(kCheapPrepend) {
    assert(readableBytes() == 0);
    assert(writableBytes() == initialSize);
    assert(prependableBytes() == kCheapPrepend);
  }

  size_t readableBytes() { return writerIndex_ - readerIndex_; }
  size_t writableBytes() { return buffer_.size() - writerIndex_; }
  size_t prependableBytes() { return readerIndex_; }

  void swap(Buffer &rhs) {
    buffer_.swap(rhs.buffer_);
    std::swap(readerIndex_, rhs.readerIndex_);
    std::swap(writerIndex_, rhs.writerIndex_);
  }

  const char *peek() { return begin() + readerIndex_; }

  // retrieve returns void, to prevent
  // string str(retrieve(readableBytes()), readableBytes());
  // the evaluation of two functions are unspecified
  void retrieve(size_t len) {
    assert(len <= readableBytes());
    if (len < readableBytes()) {
      readerIndex_ += len;
    } else {
      retrieveAll();
    }
  }

  void retrieveAll() {
    readerIndex_ = kCheapPrepend;
    writerIndex_ = kCheapPrepend;
  }

  // append
  void append(const char *data, size_t len) {
    if (writableBytes() < len) {
      makeSpace(len);
    }
    assert(writableBytes() >= len);
    std::copy(data, data + len, begin() + writerIndex_);
    writerIndex_ += len;
  }

  // preappend
  void prepend(const void *data, size_t len) {
    assert(len <= prependableBytes());
    readerIndex_ -= len;
    const char *d = static_cast<const char *>(data);
    std::copy(d, d + len, begin() + readerIndex_);
  }

  // TODO: lots of helper function

  /// Read data directly into buffer.
  ///
  /// It may implement with readv(2)
  /// @return result of read(2), @c errno is saved
  ssize_t readFd(int fd, int *savedErrno);

private:
  char *begin() { return &*buffer_.begin(); }

  void makeSpace(size_t len) {
    if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
      buffer_.resize(writerIndex_ + len);
    } else {
      // move readable data to the front, make space inside buffer
      assert(kCheapPrepend < readerIndex_);
      size_t readable = readableBytes();
      std::copy(begin() + readerIndex_, begin() + writerIndex_,
                begin() + kCheapPrepend);
      readerIndex_ = kCheapPrepend;
      writerIndex_ = readerIndex_ + readable;
      assert(readable == readableBytes());
    }
  }
  std::vector<char> buffer_;
  size_t readerIndex_;
  size_t writerIndex_;

  static const size_t kCheapPrepend = 8;
  static const size_t kInitialSize = 1024;
};

} // namespace polly

#endif //NETWORK_POLLY_UTIL_BUFFER_H_
