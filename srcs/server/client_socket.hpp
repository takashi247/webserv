#ifndef CLIENT_SOCKET_HPP
#define CLIENT_SOCKET_HPP

#include "client_info.hpp"
#include "http_request.hpp"
#include "server_socket.hpp"

class ClientSocket {
 public:
  typedef enum e_status {
    WAIT_HEADER = 0,
    PARSE_HEADER,
    WAIT_BODY,
    CREATE_RESPONSE,
    WAIT_SEND,
    WAIT_CLOSE
  } t_status;
  t_status status_;

  void ChangeStatus(t_status st) { status_ = st; }

 public:
  int fd_;
  const ServerSocket *parent_;
  t_client_info info_;
  time_t last_access_;
  std::string recv_str_;
  HttpRequest request_;
  std::string server_response_;

  ClientSocket(int fd, const ServerSocket *parent);
  ClientSocket(const ClientSocket &other);
  ~ClientSocket() {}
  ClientSocket &operator=(const ClientSocket &other);

  void Init();
};

#endif
