#ifndef LOCATIONCONFIG_HPP_
#define LOCATIONCONFIG_HPP_

#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>
#include "parser_utils.hpp"

class LocationConfig {
 public:
  LocationConfig();
  ~LocationConfig();
  LocationConfig(const LocationConfig &rhs);
  LocationConfig &operator=(const LocationConfig &rhs);

  void ParseErrorPagePath(const std::vector< std::pair< int, std::string > > &list);
  void PrintVal();

  std::string location_path_;
  std::vector<std::string> vec_accepted_method_;
  std::string rewrite_;
  std::string root_;
  bool autoindex_;
  std::vector<std::string> vec_index_;
  std::vector<std::string> vec_cgi_file_extension_;
  bool is_uploadable_;
  std::string upload_dir_;
  std::map<int, std::string> map_error_page_path_;
  size_t client_max_body_size_;

 private:
  void Init();
};

#endif
