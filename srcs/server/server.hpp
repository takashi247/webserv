#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>
#include <cstring>

#include "config.hpp"
#include "socket.hpp"

class Server {
 private:
  static const int kReadBufferSize = 1024;
  static const int kMaxSessionNum = 10;

  Config config_;
  std::vector< Socket > sockets_;
  std::vector< ClientSocket > clients_;

  void CreateServerSockets();
  int SetStartFds(fd_set *p_fds);
  int AcceptNewClient(const fd_set *fds);
  ServerConfig *FindServerConfig(int fd);
  std::string ReadMessage(int *p_fd);

 public:
  Server();  //削除予定
  Server(const char *conf);
  ~Server(){};

  void Run();
};

#endif
