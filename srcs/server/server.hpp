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

 public:
  Server();  //削除予定
  Server(const char *conf);
  ~Server(){};

  void Run();
};

#endif
