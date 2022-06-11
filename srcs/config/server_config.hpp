#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <vector>
#include "location_config.hpp"

class ServerConfig {
 public:
  ServerConfig();
  ServerConfig(LocationConfig const &lc); // for debug
  ~ServerConfig();
  ServerConfig(const ServerConfig &rhs);
  ServerConfig &operator=(const ServerConfig &rhs);

  int port_;
  std::string host_;
  std::vector<std::string> server_names_;
  std::string error_page_path_;
  int client_max_body_size_;
  std::vector<LocationConfig> vec_location_config_;

 private:
  void init();
};

#endif
