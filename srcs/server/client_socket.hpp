#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "server_socket.hpp"

class ClientSocket {
 public:
  int fd_;
  const ServerSocket* parent_;
  ClientSocket(int fd, const ServerSocket* parent) : fd_(fd), parent_(parent) {
    SetNonBlocking(fd);
  }
};

#endif
