#include "server_config.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "config.hpp"

void ServerConfig::init() {
  port_ = 0;
  client_max_body_size_ = 1024;

  line = 0;
}

ServerConfig::ServerConfig() { init(); }

ServerConfig::ServerConfig(LocationConfig const &lc)
    : vec_location_config_(1, lc) {
  init();
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

LocationConfig ServerConfig::CreateLocationConfig(std::stringstream &ss) {
  LocationConfig lc;
  std::string item;

  if (!getline(ss, item, ' ')) {
    std::cout << "Localconfig error" << std::endl;
    std::exit(1);
  }
  lc.location_path_ = item;
  if (!getline(ss, item, ' ') || item != "{") {
    std::cout << "Localconfig error" << std::endl;
    std::exit(1);
  }
  lc.ParseLocation(ss);
  return (lc);
}

int ServerConfig::ParseServer(std::string context) {
  std::stringstream ss(context);
  std::string directive;

  while (std::getline(ss, directive, ';')) {
    ParseDirective(directive);
  }
  PrintVal();
  return (0);
}

int ServerConfig::ParseDirective(std::string directive) {
  std::replace(directive.begin(), directive.end(), '\n', ' ');
  std::replace(directive.begin(), directive.end(), '\r', ' ');
  std::replace(directive.begin(), directive.end(), '\t', ' ');

  size_t pos_start = directive.find_first_not_of(' ');
  if (pos_start == std::string::npos) {
    return (1);
  }
  std::stringstream ss(directive.substr(pos_start));
  std::string item;

  if (!getline(ss, item, ' ')) {
    return (1);
  }
  if (item == "listen") {
    ParseListen(ss);
  } else if (item == "server_name") {
    vec_server_names_ = ParseVector(ss);
  } else if (item == "error_page") {
    error_page_path_ = ParseString(ss);
  } else if (item == "client_max_body_size") {
    client_max_body_size_ = ParseInt(ss, 65536);
  } else if (item == "location") {
    vec_location_config_.push_back(CreateLocationConfig(ss));
  }
  return (0);
}

int ServerConfig::ParseListen(std::stringstream &ss) {
  std::string set_value;

  if (!getline(ss, set_value, ':')) {
    return (1);
  }
  host_ = set_value;
  if (!getline(ss, set_value)) {
    return (1);
  }
  port_ = atoi(set_value.c_str());
  if (port_ > 65535 || !ss.eof()) {
    return (1);
  }
  return (0);
}

int ServerConfig::ParseServerName(std::stringstream &ss) {
  for (std::string set_value; getline(ss, set_value, ' ');) {
    vec_server_names_.push_back(set_value);
  }
  if (vec_server_names_.size() == 0) {
    return (1);
  }
  return (0);
}

int ServerConfig::ParseErrorPage(std::stringstream &ss) {
  std::string set_value;

  if (!getline(ss, set_value, ' ')) {
    return (1);
  }
  error_page_path_ = set_value;
  if (!ss.eof()) {
    return (1);
  }
  return (0);
}

int ServerConfig::ParseClientMaxBodySize(std::stringstream &ss) {
  std::string set_value;

  if (!getline(ss, set_value, ' ')) {
    return (1);
  }
  client_max_body_size_ = atoi(set_value.c_str());
  if (!ss.eof()) {
    return (1);
  }
  return (0);
}

void ServerConfig::PrintVal() {
  std::cout << "listen " << host_ << ":" << port_ << std::endl;

  std::cout << "server_name ";
  for (std::vector<std::string>::iterator it = vec_server_names_.begin();
       it != vec_server_names_.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;

  std::cout << "error_page " << error_page_path_ << std::endl;

  std::cout << "client_max_body_size " << client_max_body_size_ << std::endl;

  std::cout << "location ";
  for (std::vector<LocationConfig>::iterator it = vec_location_config_.begin();
       it != vec_location_config_.end(); ++it) {
    it->PrintVal();
  }
}
