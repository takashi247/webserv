#include "client_socket.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include <iostream>

ClientSocket::ClientSocket(int fd, const ServerSocket *parent)
    : fd_(fd), parent_(parent) {
  SetNonBlocking(fd_);
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);

  if (-1 == getpeername(fd, (struct sockaddr *)&sin, &len)) {
    std::cout << "ERROR getpeername fail!!!" << std::endl;
    return;
  }
  struct hostent *peer_host;
  if (!(peer_host = gethostbyaddr((char *)&sin.sin_addr.s_addr,
                                  sizeof(sin.sin_addr), AF_INET))) {
    std::cout << "peer_host is null" << std::endl;
    return;
  }
  info_.hostname_.assign(peer_host->h_name);
  info_.ipaddr_.assign(inet_ntoa(sin.sin_addr));
  info_.port_ = ntohs(sin.sin_port);

  std::cout << "接続: " << info_.hostname_ << "(" << info_.ipaddr_ << ")ポート "
            << info_.port_ << " ディスクリプタ " << fd << " 番\n ";
}

ClientSocket::ClientSocket(const ClientSocket &other) { *this = other; }

ClientSocket &ClientSocket::operator=(const ClientSocket &other) {
  status_ = other.status_;
  fd_ = other.fd_;
  parent_ = other.parent_;
  info_ = other.info_;
  last_access_ = other.last_access_;
  return *this;
}

void ClientSocket::Init() {
  recv_str_.clear();
  ChangeStatus(WAIT_HEADER);
  // request_.clear();
  time(&last_access_);
}
