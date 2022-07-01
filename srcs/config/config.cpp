#include "config.hpp"

Config::Config(const char *config_file) : config_file_(config_file) {
  if (config_file != NULL) {
    ConfigParser cp;

    cp.ParseConfigFile(vec_server_config_, config_file);
  }
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

const ServerConfig *Config::SelectServerConfig(const std::string &host,
                                         const size_t &port, const std::string &server_name) {
  // check if port and host match the config 
  for (std::vector<ServerConfig>::iterator it = vec_server_config_.begin();
       it < vec_server_config_.end(); ++it) {
    if (port == it->port_ && host == it->host_ &&
        std::find(it->vec_server_names_.begin(), it->vec_server_names_.end(),
                  server_name) != it->vec_server_names_.end()) {
      return (it.base());
    }
  }
  // check if port and host match the config and server_name is empty
  for (std::vector<ServerConfig>::iterator it = vec_server_config_.begin();
       it < vec_server_config_.end(); ++it) {
    if (port == it->port_ && host == it->host_ &&
        it->vec_server_names_.empty()) {
      return (it.base());
    }
  }
  // check if just port and host matches
  for (std::vector<ServerConfig>::iterator it = vec_server_config_.begin();
       it < vec_server_config_.end(); ++it) {
    if (port == it->port_ && host == it->host_) {
      return (it.base());
    }
  }
  return (&vec_server_config_[0]);
}
