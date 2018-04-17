// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NETWORK_POLLY_NET_ACCEPTOR_H_
#define NETWORK_POLLY_NET_ACCEPTOR_H_

#include <functional>
#include "socket.h"
#include "channel.h"
#include "inetaddress.h"

namespace polly {

class EventLoop;

class Acceptor {
  using NewConnectionCallback = std::function<void(int, const InetAddress &)>;

public:
  Acceptor(EventLoop *loop, const InetAddress &listen_addr);

  Acceptor(Acceptor const &) = delete;
  Acceptor &operator=(Acceptor const &) = delete;

  ~Acceptor() = default;

  void SetNewConnectionCallback(const NewConnectionCallback &cb) {
    new_conn_callback_ = cb;
  }

  void listen();

private:
  void handleRead();

  EventLoop *loop_;
  Socket listen_socket_;
  Channel listen_channel_;
  NewConnectionCallback new_conn_callback_;
};

} // namespace polly

#endif //NETWORK_POLLY_NET_ACCEPTOR_H_
