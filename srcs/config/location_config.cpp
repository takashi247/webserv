#include "location_config.hpp"

void LocationConfig::Init() {
  vec_accepted_method_.push_back("GET");
  vec_accepted_method_.push_back("POST");
  vec_accepted_method_.push_back("DELETE");
  autoindex_ = false;
  vec_index_.push_back("index.html");
  is_uploadable_ = false;
  client_max_body_size_ = 1024;
}

LocationConfig::LocationConfig() { Init(); }

LocationConfig::~LocationConfig() {}

LocationConfig::LocationConfig(const LocationConfig &rhs) { *this = rhs; }

LocationConfig &LocationConfig::operator=(const LocationConfig &rhs) {
  if (this != &rhs) {
    location_path_ = rhs.location_path_;
    vec_accepted_method_ = rhs.vec_accepted_method_;
    rewrite_ = rhs.rewrite_;
    root_ = rhs.root_;
    autoindex_ = rhs.autoindex_;
    vec_index_ = rhs.vec_index_;
    vec_cgi_file_extension_ = rhs.vec_cgi_file_extension_;
    is_uploadable_ = rhs.is_uploadable_;
    upload_dir_ = rhs.upload_dir_;
    map_error_page_path_ = rhs.map_error_page_path_;
    client_max_body_size_ = rhs.client_max_body_size_;
  }
  return *this;
}

void LocationConfig::ParseErrorPagePath(
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
    ParserUtils::AtoSizeT(it->second.c_str(), list, i);
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

void LocationConfig::PrintVal() {
  std::cout << "location " << location_path_ << std::endl;

  std::cout << "http_method ";
  for (std::vector<std::string>::iterator it = vec_accepted_method_.begin();
       it != vec_accepted_method_.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;

  std::cout << "rewrite " << rewrite_ << std::endl;

  std::cout << "root " << root_ << std::endl;

  std::cout << "autoindex " << autoindex_ << std::endl;

  std::cout << "index ";
  for (std::vector<std::string>::iterator it = vec_index_.begin();
       it != vec_index_.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;

  std::cout << "add_cgi_handler ";
  for (std::vector<std::string>::iterator it = vec_cgi_file_extension_.begin();
       it != vec_cgi_file_extension_.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;

  std::cout << "is_uploadable " << is_uploadable_ << std::endl;

  std::cout << "upload_store  " << upload_dir_ << std::endl;

  std::cout << "error_page ";
  for (std::map< int, std::string >::iterator it = map_error_page_path_.begin();
       it != map_error_page_path_.end(); ++it) {
    std::cout << it->first << " " << it->second;
  }
  std::cout << std::endl;

  std::cout << "client_max_body_size " << client_max_body_size_ << std::endl;
}
