#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "client_info.hpp"
#include "http_request.hpp"
#include "server_socket.hpp"

class ServerConfig;
class Config;

class ClientSocket {
 private:
  typedef enum e_status {
    WAIT_HEADER = 0,
    PARSE_HEADER,
    WAIT_BODY,
    CREATE_RESPONSE,
    WAIT_SEND,
    WAIT_CLOSE
  } t_status;
  t_status status_;

  static const int kDisconnectSec = 60;
  int fd_;
  const ServerSocket *parent_;
  t_client_info info_;

  std::string recv_str_;
  HttpRequest request_;
  std::string server_response_;
  time_t last_access_;
  size_t parsed_pos_;
  size_t chunked_remain_size_;

 public:
  ClientSocket(int fd, const ServerSocket *parent, struct sockaddr_in &sin);
  ClientSocket(const ClientSocket &other);
  ~ClientSocket() {}
  ClientSocket &operator=(const ClientSocket &other);

 private:
  void ChangeStatus(t_status st) { status_ = st; }
  int ReceiveHeader();
  int ParseChunkedBody();
  int ReceiveBody();
  int SendMessage();

 public:
  void Init();
  int GetFd() { return fd_; }
  int EventHandler(bool is_readable, bool is_writable, Config &config);
};

#endif
