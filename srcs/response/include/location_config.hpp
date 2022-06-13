#ifndef LOCATION_CONFIG_HPP_
#define LOCATION_CONFIG_HPP_

#include <string>
#include <vector>

class LocationConfig {
 public:
  std::string location_path_;
  std::vector<std::string> vec_accepted_method_;
  std::string proxy_path_;
  std::string root_;
  bool autoindex_;
  std::vector<std::string> vec_index_;
  std::vector<std::string> vec_cgi_file_extension_;
  bool is_uploadable_;
  std::string upload_dir_;
};

#endif // LOCATION_CONFIG_HPP_
