#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

#include "location_config.hpp"
#include "config_parser.hpp"

class Config {
 public:
  Config(std::string config_file = "");
  ~Config();
  Config(Config const& rhs);
  Config& operator=(Config const& rhs);

  // host_(listen), port_(listen), server_namesと照合する。ない場合は一番最初のものを返す
  ServerConfig* SelectServerConfig(const std::string& host, const size_t& port,
                                   const std::string& server_name);

  std::string config_file_;
  std::vector<ServerConfig> vec_server_config_;
};

#endif
