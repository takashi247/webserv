#include "location_config.hpp"

#include <iostream>
#include <sstream>

#include "config.hpp"

void LocationConfig::init() {
  vec_accepted_method_.push_back("GET");
  vec_accepted_method_.push_back("POST");
  vec_accepted_method_.push_back("DELETE");
  autoindex_ = false;
  vec_index_.push_back("index.html");
  is_uploadable_ = false;
}

LocationConfig::LocationConfig() { init(); }

LocationConfig::~LocationConfig() {}

LocationConfig::LocationConfig(const LocationConfig &rhs) { *this = rhs; }

LocationConfig &LocationConfig::operator=(const LocationConfig &rhs) {
  if (this != &rhs) {
    location_path_ = rhs.location_path_;
    vec_accepted_method_ = rhs.vec_accepted_method_;
    proxy_pass_ = rhs.proxy_pass_;
    root_ = rhs.root_;
    autoindex_ = rhs.autoindex_;
    vec_index_ = rhs.vec_index_;
    vec_cgi_file_extension_ = rhs.vec_cgi_file_extension_;
    is_uploadable_ = rhs.is_uploadable_;
    upload_dir_ = rhs.upload_dir_;
  }
  return *this;
}

void LocationConfig::PrintVal() {
  std::cout << "location " << location_path_ << std::endl;

  std::cout << "http_method ";
  for (std::vector<std::string>::iterator it = vec_accepted_method_.begin();
       it != vec_accepted_method_.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;

  std::cout << "proxy_pass " << proxy_pass_ << std::endl;

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
}
