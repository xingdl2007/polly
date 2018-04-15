// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <stdio.h>
#include <assert.h>
#include "log_stream.h"

namespace polly {

LogStream::self &LogStream::operator<<(bool v) {
  std::string msg = v ? "1" : "0";
  Slice s(msg);
  append(s);
  return *this;
}

LogStream::self &LogStream::operator<<(char v) {
  return *this << Fmt("%c", v);
}

LogStream::self &LogStream::operator<<(unsigned char v) {
  return *this << Fmt("%uc", v);
}

LogStream::self &LogStream::operator<<(short v) {
  return *this << Fmt("%d", v);
}

LogStream::self &LogStream::operator<<(unsigned short v) {
  return *this << Fmt("%d", v);
}

LogStream::self &LogStream::operator<<(int v) {
  return *this << Fmt("%d", v);
}

LogStream::self &LogStream::operator<<(unsigned int v) {
  return *this << Fmt("%d", v);
}

LogStream::self &LogStream::operator<<(long v) {
  return *this << Fmt("%ld", v);
}

LogStream::self &LogStream::operator<<(unsigned long v) {
  return *this << Fmt("%uld", v);
}

LogStream::self &LogStream::operator<<(long long v) {
  return *this << Fmt("%lld", v);
}

LogStream::self &LogStream::operator<<(unsigned long long v) {
  return *this << Fmt("%ulld", v);
}

LogStream::self &LogStream::operator<<(float v) {
  return *this << Fmt("%f", v);
}

LogStream::self &LogStream::operator<<(double v) {
  return *this << Fmt("%f", v);
}

LogStream::self &LogStream::operator<<(const char *s) {
  Slice slice(s);
  append(slice);
  return *this;
}

LogStream::self &LogStream::operator<<(std::string const &s) {
  Slice slice(s);
  append(slice);
  return *this;
}

LogStream::self &LogStream::operator<<(Slice const &s) {
  append(s);
  return *this;
}

template<typename... T>
Fmt::Fmt(const char *fmt, T... val) {
  length_ = static_cast<size_t >(snprintf(buf_, sizeof buf_, fmt, val...));
  assert(length_ < sizeof buf_);
}

// Explicit instantiations
template Fmt::Fmt(const char *fmt, char);

template Fmt::Fmt(const char *fmt, short);
template Fmt::Fmt(const char *fmt, unsigned short);
template Fmt::Fmt(const char *fmt, int);
template Fmt::Fmt(const char *fmt, unsigned int);
template Fmt::Fmt(const char *fmt, long);
template Fmt::Fmt(const char *fmt, unsigned long);
template Fmt::Fmt(const char *fmt, long long);
template Fmt::Fmt(const char *fmt, unsigned long long);

template Fmt::Fmt(const char *fmt, float);
template Fmt::Fmt(const char *fmt, double);

} // namespace polly