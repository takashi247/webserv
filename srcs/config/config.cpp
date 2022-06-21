#include "config.hpp"

Config::Config(std::string config_file) : config_file_(config_file) {
  ConfigParser cp;

  cp.ParseConfigFile(vec_server_config_, config_file.c_str());
}

Config::~Config() {}

Config::Config(Config const &rhs) { *this = rhs; }

Config &Config::operator=(Config const &rhs) {
  if (this != &rhs) {
    vec_server_config_ = rhs.vec_server_config_;
    config_file_ = rhs.config_file_;
  }
  return *this;
}

ServerConfig *Config::SelectServerConfig(const std::string &host,
                                         const size_t &port) {
  // check if port and host match the config
  for (std::vector<ServerConfig>::iterator it = vec_server_config_.begin();
       it < vec_server_config_.end(); ++it) {
    if (port == it->port_ &&
        std::find(it->vec_server_names_.begin(), it->vec_server_names_.end(),
                  host) != it->vec_server_names_.end()) {
      return (it.base());
    }
  }
  // check if port matches the config and server_name is empty
  for (std::vector<ServerConfig>::iterator it = vec_server_config_.begin();
       it < vec_server_config_.end(); ++it) {
    if (port == it->port_ && it->vec_server_names_.empty()) {
      return (it.base());
    }
  }
  return (&vec_server_config_[0]);
}
