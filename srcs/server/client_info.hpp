#ifndef CLIENT_INFO_HPP
#define CLIENT_INFO_HPP

#include <string>

typedef struct s_client_info {
  std::string hostname;
  std::string ipaddr;
  int port;
} t_client_info;

#endif
