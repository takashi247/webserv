#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "client_info.hpp"
#include "server_socket.hpp"

class ClientSocket {
 public:
  int fd_;
  const ServerSocket *parent_;
  t_client_info info_;
  time_t last_access_;
  ClientSocket(int fd, const ServerSocket *parent);
  ClientSocket(const ClientSocket &other);
  ~ClientSocket() {}
  ClientSocket &operator=(const ClientSocket &other);

  void Init();
};

#endif
