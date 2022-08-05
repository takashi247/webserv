#include "server_config.hpp"

void ServerConfig::Init() { port_ = 80; }

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
    vec_location_config_ = rhs.vec_location_config_;
    default_location_config_ = rhs.default_location_config_;
  }
  return (*this);
}

void ServerConfig::ParseListen(
    const std::vector< std::pair< int, std::string > > &list) {
  std::string::size_type delim_pos;
  static const size_t port_max = (1 << 16) - 1;  

  if (list.size() != 2) {
    ParserUtils::MakeUnexpected(
        "invalid number of args in \"listen\" directive", list[0].first);
  }
  std::string set_value = list[1].second;
  delim_pos = set_value.find(':');
  // make error message again(number => port)
  try {
    if (delim_pos == std::string::npos) {
      if (set_value.find('.') == std::string::npos) {
        ParserUtils::AtoSizeT(set_value.c_str(), list, port_);
      } else {
        host_ = set_value;
      }
    } else {
      host_ = set_value.substr(0, delim_pos);
      ParserUtils::AtoSizeT(set_value.substr(delim_pos + 1).c_str(), list, port_);
    }
  } catch (const WebservException &e) {
    ParserUtils::MakeUnexpected("invalid port specified in \"" + list[0].second + "\" directive", list[1].first);
  }
  if (port_ == 0 || port_ > port_max) {
    ParserUtils::MakeUnexpected("invalid port specified in \"" + list[0].second + "\" directive", list[1].first);
  }
}

// return locationConfig whose location_path_'s length is longest.
const LocationConfig *ServerConfig::SelectLocationConfig(
    const std::string &uri) const {
  const LocationConfig *selected = &default_location_config_;

  for (std::vector< LocationConfig >::const_iterator it =
           vec_location_config_.begin();
       it != vec_location_config_.end(); ++it) {
    if (uri.find(it->location_path_) == 0) {
      if (selected == &default_location_config_) {
        selected = it.base();
      } else if (selected->location_path_.length() <
                 it->location_path_.length()) {
        selected = it.base();
      }
    }
  }
  return (selected);
}

std::string ServerConfig::UpdateUri(const std::string &uri) const {
  std::string path;

  const LocationConfig *lc = SelectLocationConfig(uri);
  if (lc->rewrite_.empty()) {
    path = lc->root_ + uri;
  } else {
    path = (lc->location_path_ == "/")
               ? uri
               : uri.substr(lc->location_path_.length());
    path = lc->rewrite_ + path;
  }

  struct stat buffer;
  if (*(path.end() - 1) == '/') {
    for (std::vector< std::string >::const_iterator it = lc->vec_index_.begin();
         it != lc->vec_index_.end(); ++it) {
      if (stat((path + *it).c_str(), &buffer) == 0) {
        return (path + *it);
      }
    }
  }
  return (path);
}

void ServerConfig::PrintVal() {
  std::cout << "=====" << std::endl;
  std::cout << "listen " << host_ << ":" << port_ << std::endl;

  std::cout << "server_name ";
  for (std::vector< std::string >::iterator it = vec_server_names_.begin();
       it != vec_server_names_.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;

  for (std::vector< LocationConfig >::iterator it =
           vec_location_config_.begin();
       it != vec_location_config_.end(); ++it) {
    std::cout << "\n[location]" << std::endl;
    it->PrintVal();
  }
  std::cout << "=====" << std::endl;
}
