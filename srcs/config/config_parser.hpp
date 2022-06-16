#ifndef CONFIG_PARSER_HPP_
#define CONFIG_PARSER_HPP_

#include <algorithm>
#include <fstream>
#include <iostream>

#include "config.hpp"
#include "parser_utils.hpp"

class ConfigParser {
 public:
  ConfigParser();
  ConfigParser(ConfigParser const &rhs);
  ~ConfigParser();
  ConfigParser &operator=(ConfigParser const &rhs);

  void ParseConfigFile(Config &config);

 private:
  void Tokenize(const char *file_name);
  void Parse(Config &config);
  void BalanceBraces();
  void SplitIntoList(std::vector<std::pair<int, std::string> > &list);
  ServerConfig CreateServerConfig();
  LocationConfig CreateLocationConfig();

  std::vector<std::pair<int, std::string> > tokens_;
  size_t index_;
};

#endif
