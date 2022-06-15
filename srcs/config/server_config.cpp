#include "server_config.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "config.hpp"

void ServerConfig::Init() {
  port_ = 0;
  client_max_body_size_ = 1024;
}

ServerConfig::ServerConfig() { Init(); }

ServerConfig::ServerConfig(LocationConfig const &lc)
    : vec_location_config_(1, lc) {
  Init();
}

ServerConfig::~ServerConfig() {}

ServerConfig::ServerConfig(ServerConfig const &rhs) { *this = rhs; }

ServerConfig &ServerConfig::operator=(ServerConfig const &rhs) {
  if (this != &rhs) {
    port_ = rhs.port_;
    host_ = rhs.host_;
    vec_server_names_ = rhs.vec_server_names_;
    error_page_path_ = rhs.error_page_path_;
    client_max_body_size_ = rhs.client_max_body_size_;
    vec_location_config_ = rhs.vec_location_config_;
  }
  return *this;
}

void ServerConfig::ParseListen(const std::vector<std::pair<int, std::string> > &list) {
  std::string::size_type delim_pos;

  if (list.size() != 2) {
    MakeUnexpected("invalid number of args in listen directive", list[0].first);
  }
  std::string set_value = list[1].second;
  delim_pos = set_value.find(':');
  if (delim_pos == std::string::npos) {
    port_ = std::atoi(set_value.c_str());
  } else {
    host_ = set_value.substr(0, delim_pos);
    port_ = std::atoi(set_value.substr(delim_pos + 1).c_str());
  }
}

void ServerConfig::PrintVal() {
  std::cout << "=====" << std::endl;
  std::cout << "listen " << host_ << ":" << port_ << std::endl;

  std::cout << "server_name ";
  for (std::vector<std::string>::iterator it = vec_server_names_.begin();
       it != vec_server_names_.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;

  std::cout << "error_page " << error_page_path_ << std::endl;

  std::cout << "client_max_body_size " << client_max_body_size_ << std::endl;

  std::cout << "[location]" << std::endl;
  for (std::vector<LocationConfig>::iterator it = vec_location_config_.begin();
       it != vec_location_config_.end(); ++it) {
    it->PrintVal();
  }
  std::cout << "=====" << std::endl;
}
