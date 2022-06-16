#ifndef SERVER_HPP
#define SERVER_HPP

#include <map>
#include <vector>

#include "config.hpp"
#include "socket.hpp"

class Server {
 private:
  Config config_;
  std::vector<Socket *> sockets_;
  static const int kReadBufferSize = 1024;
  static const int kMaxSessionNum = 10;

  int AcceptNewClient(const fd_set *fds, int *accfd);
  std::string ReadMessage(int *p_fd);

 public:
  Server();  //削除予定
  Server(const char *conf);
  ~Server(){};

  void Run();
};

#endif
