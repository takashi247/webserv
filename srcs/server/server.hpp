#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>

#include "config.hpp"
#include "server_socket.hpp"

class ClientSocket;

class Server {
 private:
  static const int kMaxSessionNum = 512;

  Config config_;
  std::vector< ServerSocket > sockets_;
  std::vector< ClientSocket * > clients_;

  void CreateServerSockets();
  int SetStartFds(fd_set &r_fds, fd_set &w_fds);
  int AcceptNewClient(const fd_set &fds);

 public:
  Server();
  Server(const char *conf);
  ~Server(){};

  void Run();
};

#endif
