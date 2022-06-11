#ifndef SERVER_CONFIG_HPP_
#define SERVER_CONFIG_HPP_

#include "location_config.hpp"

#include <string>
#include <vector>

class ServerConfig {
 public:
  unsigned int port_; // The highest TCP port number is 65535
  std::string host_;
  std::string server_name_;
  std::string error_page_path_;
  int client_max_body_size_; // @fyuta: ConfigParserの方で数字に読み替えてもらえるとありがたいです。intで収まり切るかどうかは要検討
  std::vector<LocationConfig> vec_location_config_;
};

#endif // SERVER_CONFIG_HPP_
