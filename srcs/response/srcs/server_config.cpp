#include "server_config.hpp"

std::string ServerConfig::UpdateUri(std::string uri) {
  return uri;
}

LocationConfig* ServerConfig::SelectLocationConfig(const std::string& uri) {
  (void)uri;
  return NULL;
}
