#ifndef CONFIGPARSER_HPP_
#define CONFIGPARSER_HPP_

#include <string>
#include <vector>
#include <set>
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
  std::vector<std::string> vec_server_names_;
  std::string error_page_path_;
  int client_max_body_size_;
  std::vector<LocationConfig> vec_location_config_;

  LocationConfig CreateLocationConfig(std::stringstream &ss);
  int ParseServer(std::string context);
  void PrintVal();
 private:
  int line;

  void init();
  int ParseDirective(std::string directive);
  int ParseListen(std::stringstream& ss);
  int ParseServerName(std::stringstream& ss);
  int ParseErrorPage(std::stringstream& ss);
  int ParseClientMaxBodySize(std::stringstream& ss);

};

#endif
