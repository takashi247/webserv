#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <netinet/in.h>

class Socket {
  int listenfd_;
  int port_;
  struct sockaddr_in serv_addr_;

 public:
  explicit Socket(int port) : port_(port) {}
  ~Socket() {}

 private:
  void SetListenfd();
  void SetSockaddrIn();

 public:
  int SetSocket();
  int GetListenFd() const { return this->listenfd_; }
};

struct ClientSocket {
  int listenfd_;
  int fd_;
};
#endif
