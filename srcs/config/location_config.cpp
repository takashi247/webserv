#include "location_config.hpp"
#include <sstream>
#include "config.hpp"
void LocationConfig::init()
{
  vec_accepted_method_.push_back("GET");
  vec_accepted_method_.push_back("POST");
  vec_accepted_method_.push_back("DELETE");
  autoindex_ = false;
  vec_index_.push_back("index.html");
  is_uploadable_ = false;

}

LocationConfig::LocationConfig() {
  init();
}

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

int LocationConfig::ParseLocation(std::stringstream &ss) {
  std::string directive;
  
  while (std::getline(ss, directive, ';')) {
    ParseDirective(directive);
    
  }

  PrintVal();
  return (0);
}

int LocationConfig::ParseDirective(std::string directive) {
  std::string item;
  std::stringstream ss(directive);

  if (!std::getline(ss, item, ' ')) {
    return (1);
  }
  if (item == "http_method") {
    vec_accepted_method_ = ParseVector(ss);
  } else if (item == "proxy_path") {
    proxy_pass_ = ParseString(ss);
  } else if (item == "root") {

  }
}


void LocationConfig::PrintVal() {

  return ;
}

