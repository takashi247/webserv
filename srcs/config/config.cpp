#include "config.hpp"

Config::Config(std::string config_file) : config_file_(config_file) {}

Config::~Config() {}

Config::Config(Config const &rhs) { *this = rhs; }

Config &Config::operator=(Config const &rhs) {
  if (this != &rhs) {
    vec_server_config_ = rhs.vec_server_config_;
    config_file_ = rhs.config_file_;
  }
  return *this;
}

// return serverConfig which matches host, port and server_name
ServerConfig *Config::SelectServerConfig(const std::string &host,
                                         const size_t &port,
                                         const std::string &server_name) {
  for (std::vector<ServerConfig>::iterator it = vec_server_config_.begin();
       it < vec_server_config_.end(); ++it) {
    if ((it->host_ == "" || host == it->host_) && port == it->port_) {
      if (std::find(it->vec_server_names_.begin(), it->vec_server_names_.end(),
                    server_name) != it->vec_server_names_.end()) {
        return (it.base());
      }
    }
  }
  return (&vec_server_config_[0]);
}
