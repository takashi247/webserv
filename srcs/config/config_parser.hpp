#ifndef CONFIG_PARSER_HPP_
#define CONFIG_PARSER_HPP_

#include "config.hpp"
#include "location_config.hpp"

class ConfigParser {
 private:
  std::vector<std::pair<int, std::string> > tokens_;
  size_t index_;

  void Tokenize(const char *file_name);
  void Parse(Config &config);
  void BalanceBraces();
  void SplitIntoList(std::vector<std::pair<int, std::string> > &list);
  ServerConfig CreateServerConfig();
  LocationConfig CreateLocationConfig();

 public:
  ConfigParser();
  ConfigParser(ConfigParser const &rhs);
  ~ConfigParser();
  ConfigParser &operator=(ConfigParser const &rhs);

  void ParseConfigFile(Config &config);
};

#endif
