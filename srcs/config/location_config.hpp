#ifndef LOCATIONCONFIG_HPP_
#define LOCATIONCONFIG_HPP_

#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class LocationConfig {
 public:
  LocationConfig();
  ~LocationConfig();
  LocationConfig(const LocationConfig &rhs);
  LocationConfig &operator=(const LocationConfig &rhs);

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

 private:
  void Init();
};

#endif
