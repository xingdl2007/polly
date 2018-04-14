// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_UTIL_BUFFER_H_
#define NETWORK_POLLY_UTIL_BUFFER_H_

#include <string.h>
#include <include/polly/slice.h>

namespace polly {

const int kSmallBufferSize = 4096;

template<int N>
class FixedBuffer {
public:
  FixedBuffer() : cur_(data) { bzero(); }

  FixedBuffer(FixedBuffer const &) = delete;
  FixedBuffer &operator=(FixedBuffer const &) = delete;

  ~FixedBuffer() = default;

  // available space
  size_t space() { return static_cast<size_t >(end() - cur_); }

  void bzero() { ::bzero(data, sizeof data); }

  // after last element pointer
  char *end() { return data + N; }

  void append(Slice const &s) {
    assert(space() >= s.size());
    ::memcpy(cur_, s.data(), s.size());
    cur_ += s.size();
  }

private:
  char data[N];
  char *cur_;
};

// explicit instantiations
template class FixedBuffer<kSmallBufferSize>;

} // namespace polly

#endif //NETWORK_POLLY_UTIL_BUFFER_H_
