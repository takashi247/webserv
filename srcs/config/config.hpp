#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <sstream>
#include <string>
#include <vector>

#include "server_config.hpp"

struct Config {
  std::vector<ServerConfig> vec_server_config_;
  std::string config_file_;
};

void MakeUnexpected(const std::string& msg, const int& pos);
void ParseInt(const std::vector<std::pair<int, std::string> >& list, size_t& i);
void ParseBool(const std::vector<std::pair<int, std::string> >& list, bool& b);
void ParseString(const std::vector<std::pair<int, std::string> >& list,
                 std::string& str);
void ParseVector(const std::vector<std::pair<int, std::string> >& list,
                 std::vector<std::string>& str);

#endif
