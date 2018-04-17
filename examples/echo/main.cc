// Copyright (c) 2018 The Polly Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <unistd.h>
#include <fcntl.h>
#include <fstream>

int main() {
  // flush does not call fsync, just like fflush()
  std::fstream file("test.log");
  file << "from file stream";
  file.flush();
  file.close();

  // fsync
  char buffer[32] = "hello test.log\n";
  int fd = ::open("test.log", O_WRONLY, "ae");
  ::write(fd, buffer, sizeof buffer);
  ::fsync(fd);
  ::close(fd);
  return 0;
}
