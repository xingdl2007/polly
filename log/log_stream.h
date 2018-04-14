// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_LOG_LOG_STREAM_H_
#define NETWORK_POLLY_LOG_LOG_STREAM_H_

#include <util/buffer.h>
#include <include/polly/slice.h>

namespace polly {

class LogStream {
  typedef LogStream self;
public:
  LogStream() = default;

  LogStream(LogStream const &) = delete;
  LogStream &operator=(LogStream const &) = delete;

  ~LogStream() = default;

  void append(Slice const &s) {
    buffer_.append(s);
  }

  const char *data() { return buffer_.data(); }

  self &operator<<(bool);
  self &operator<<(char);
  self &operator<<(unsigned char);
  self &operator<<(short);
  self &operator<<(unsigned short);
  self &operator<<(int);
  self &operator<<(unsigned int);
  self &operator<<(long);
  self &operator<<(unsigned long);
  self &operator<<(long long);
  self &operator<<(unsigned long long);
  self &operator<<(float);
  self &operator<<(double);
  self &operator<<(const char *);
  self &operator<<(std::string const &);

private:
  FixedBuffer<kSmallBufferSize> buffer_;
};

class Fmt {
public:
  template<typename... T>
  Fmt(const char *fmt, T... val);

  const char *data() const { return buf_; }
  size_t length() const { return length_; }

private:
  char buf_[32];
  size_t length_;
};

inline LogStream &operator<<(LogStream &s, const Fmt &fmt) {
  s.append(Slice(fmt.data(), fmt.length()));
  return s;
}

} // namespace polly

#endif //NETWORK_POLLY_LOG_LOG_STREAM_H_
