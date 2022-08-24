#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "client_info.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "server_socket.hpp"

class ServerConfig;
class Config;

typedef struct s_fd_acceptable {
  bool client_read;
  bool client_write;
  bool response_read;
  bool cgi_read;
  bool cgi_write;
} t_fd_acceptable;

class ClientSocket {
 private:
  typedef enum e_status {
    WAIT_HEADER = 0,
    PARSE_HEADER,
    WAIT_BODY,
    INIT_RESPONSE,
    CREATE_RESPONSE,
    WAIT_SEND,
    WAIT_CLOSE
  } t_status;
  t_status status_;

  static const int kDisconnectSec = 60;
  int fd_;
  const ServerSocket *parent_;
  t_client_info info_;
  HttpResponse response_;

  std::string recv_str_;
  HttpRequest request_;
  std::string server_response_;
  time_t last_access_;
  size_t parsed_pos_;
  size_t remain_size_;

 public:
  ClientSocket(int fd, const ServerSocket *parent, struct sockaddr_in &sin);
  ClientSocket(const ClientSocket &other);
  ~ClientSocket() {}
  ClientSocket &operator=(const ClientSocket &other);

 private:
  void ChangeStatus(t_status st) { status_ = st; }
  int ReceiveHeader();
  int ReceiveBody();

 public:
  void Init();
  int GetFd() { return fd_; }
  int GetResponseReadFd() { return response_.GetResponseReadFd(); }
  int GetCgiReadFd() { return response_.GetCgiReadFd(); }
  int GetCgiWriteFd() { return response_.GetCgiWriteFd(); }
  bool IsWaitSend() { return (status_ == WAIT_SEND); }
  bool IsCgiWriable() { return response_.IsWritingCgiInput(); }

  int EventHandler(t_fd_acceptable &ac, Config &config);
};

#endif
