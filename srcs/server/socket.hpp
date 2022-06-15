#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>

class Socket {
  int listenfd;
  int port;
  struct sockaddr_in serv_addr;

 public:
  explicit Socket(int port_) : port(port_) {}
  ~Socket() {}
  void set_listenfd();
  void set_sockaddr_in();
  int set_socket();

  int get_listenfd() const { return this->listenfd; }
};

#endif
