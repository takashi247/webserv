#ifndef CONFIG_PARSER_H_
#define CONFIG_PARSER_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "server_config.hpp"

class Config;

class ConfigParser {
 public:
  ConfigParser();
  ConfigParser(ConfigParser const &rhs);
  ~ConfigParser();
  ConfigParser &operator=(ConfigParser const &rhs);

  void ParseConfigFile(std::vector<ServerConfig> &vec_server_config,
                       const char *file_name);

 private:
  void Tokenize(const char *file_name);
  void Parse(std::vector<ServerConfig> &vec_server_config);
  void BalanceBraces();
  void ValidateServerDuplication(const std::vector<ServerConfig> &vec_server_config);
  void SplitIntoList(std::vector<std::pair<int, std::string> > &list);
  void ParseServerConfig(std::vector<ServerConfig> &vec_server_config);
  void ParseLocationConfig(std::vector<LocationConfig> &vec_location_config,
                           const LocationConfig &default_lc);

  std::vector<std::pair<int, std::string> > tokens_;
  size_t index_;
};

#endif
