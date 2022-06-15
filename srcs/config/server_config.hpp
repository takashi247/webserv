#ifndef CONFIGPARSER_HPP_
#define CONFIGPARSER_HPP_

#include <set>
#include <string>
#include <vector>

#include "location_config.hpp"

class ServerConfig {
 public:
  ServerConfig();
  ServerConfig(LocationConfig const &lc);  // for debug
  ~ServerConfig();
  ServerConfig(const ServerConfig &rhs);
  ServerConfig &operator=(const ServerConfig &rhs);

  int port_;
  std::string host_;
  std::vector<std::string> vec_server_names_;
  std::string error_page_path_;
  int client_max_body_size_;
  std::vector<LocationConfig> vec_location_config_;

  LocationConfig CreateLocationConfig(std::stringstream &ss);

  void PrintVal();
  void ParseListen(const std::vector<std::pair<int, std::string> > &list);

 private:
  void Init();
};

#endif
