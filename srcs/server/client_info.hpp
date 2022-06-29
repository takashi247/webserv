#ifndef CLIENT_INFO_HPP
#define CLIENT_INFO_HPP

#include <string>

typedef struct s_client_info {
  std::string hostname_;
  std::string ipaddr_;
  int port_;
} t_client_info;

#endif
