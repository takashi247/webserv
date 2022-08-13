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
      if (set_value.find('.') == std::string::npos && std::isdigit(set_value[0])) {
        ParserUtils::AtoSizeT(set_value.c_str(), list, port_);
      } else {
        host_ = hostToIp(set_value);
      }
    } else {
      host_ = hostToIp(set_value.substr(0, delim_pos));
      ParserUtils::AtoSizeT(set_value.substr(delim_pos + 1).c_str(), list, port_);
    }
  } catch (const WebservException &e) {
    ParserUtils::MakeUnexpected("invalid port specified in \"" + list[0].second + "\" directive", list[1].first);
  }
  if (port_ == 0 || port_ > port_max) {
    ParserUtils::MakeUnexpected("invalid port specified in \"" + list[0].second + "\" directive", list[1].first);
  }
}

in_addr_t ServerConfig::hostToIp(const std::string &str) {
  in_addr_t ip;

  ip = inet_addr(str.c_str());
  if (ip == 0xffffffff) {
    struct hostent *host;
    host = gethostbyname(str.c_str());
    if (host == NULL) {
      ParserUtils::MakeUnexpected("host not found in \"" + str + "\" in \"listen\" directive", 0);
    }
    ip = *(unsigned int *)host->h_addr_list[0];
  }
  return (ip);
}

void ServerConfig::ValidateServerDuplication(
    const std::vector< ServerConfig > &vec_sc) {
  for (std::vector< ServerConfig >::const_iterator it = vec_sc.begin();
       it != vec_sc.end(); ++it) {
    if (it->host_ == host_ && it->port_ == port_) {
      if (it->vec_server_names_.empty() && vec_server_names_.empty()) {
        std::stringstream ss;

        ss << "conflicting server name \"\" on " << host_ << ":" << port_;
        ParserUtils::MakeUnexpected(ss.str(), 0);
      }
      for (std::vector< std::string >::const_iterator serv_it =
               it->vec_server_names_.begin();
           serv_it != it->vec_server_names_.end(); ++serv_it) {
        if (std::find(vec_server_names_.begin(), vec_server_names_.end(),
                      *serv_it) != vec_server_names_.end()) {
          std::stringstream ss;
          ss << "conflicting server name \"" + *serv_it + "\" on " << host_ <<
                    ":"
             << port_;
          ParserUtils::MakeUnexpected(ss.str(), 0);
        }
      }
    }
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
