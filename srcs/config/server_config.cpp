#include "server_config.hpp"

void ServerConfig::Init() {
  port_ = 80;
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
    map_error_page_path_ = rhs.map_error_page_path_;
    client_max_body_size_ = rhs.client_max_body_size_;
    vec_location_config_ = rhs.vec_location_config_;
  }
  return (*this);
}

void ServerConfig::ParseListen(
    const std::vector< std::pair< int, std::string > > &list) {
  std::string::size_type delim_pos;

  if (list.size() != 2) {
    ParserUtils::MakeUnexpected(
        "invalid number of args in \"listen\" directive", list[0].first);
  }
  std::string set_value = list[1].second;
  delim_pos = set_value.find(':');
  if (delim_pos == std::string::npos) {
    if (set_value.find('.') == std::string::npos) {
      ParserUtils::AtoSizeT(set_value.c_str(), list[0], port_);
    } else {
      host_ = set_value;
    }
  } else {
    host_ = set_value.substr(0, delim_pos);
    ParserUtils::AtoSizeT(set_value.substr(delim_pos + 1).c_str(), list[0],
                          port_);
  }
}

void ServerConfig::ParseErrorPagePath(
    const std::vector< std::pair< int, std::string > > &list) {
  std::pair< int, std::string > elem;
  std::vector< std::pair< int, std::string > >::const_iterator it =
      list.begin();
  size_t i;

  if (list.size() < 2 || list.size() % 2 != 1) {
    ParserUtils::MakeUnexpected(
        "invalid number of args in \"error_page_path\" directive",
        list[0].first);
  }
  ++it;
  while (it != list.end()) {
    ParserUtils::AtoSizeT(it->second.c_str(), list[0], i);
    if (i > 505 || i < 100) {
      ParserUtils::MakeUnexpected(
          "invalid http status specified in \"error_page_path\" directive",
          list[0].first);
    }
    elem.first = i;
    it++;
    elem.second = it->second;
    it++;
    map_error_page_path_.insert(elem);
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

  std::cout << "error_page ";
  for (std::map< int, std::string >::iterator it = map_error_page_path_.begin();
       it != map_error_page_path_.end(); ++it) {
    std::cout << it->first << " " << it->second;
  }
  std::cout << std::endl;

  std::cout << "client_max_body_size " << client_max_body_size_ << std::endl;

  for (std::vector< LocationConfig >::iterator it =
           vec_location_config_.begin();
       it != vec_location_config_.end(); ++it) {
    std::cout << "\n[location]" << std::endl;
    it->PrintVal();
  }
  std::cout << "=====" << std::endl;
}
