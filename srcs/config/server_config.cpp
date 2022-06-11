#include "server_config.hpp"

#include <iostream>

void ServerConfig::init() {
  port_ = 0;
  client_max_body_size_ = 1024;
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
    server_names_ = rhs.server_names_;
    error_page_path_ = rhs.error_page_path_;
    client_max_body_size_ = rhs.client_max_body_size_;
    vec_location_config_ = rhs.vec_location_config_;
  }
  return *this;
}
