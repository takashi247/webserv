#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>

class LocationConfig {
 public:
  LocationConfig();
  ~LocationConfig();
  LocationConfig(const LocationConfig &rhs);
  LocationConfig &operator=(const LocationConfig &rhs);

  std::string location_path_;
  std::vector<std::string> vec_accepted_method_;
  std::string proxy_pass_;
  std::string root_;
  bool autoindex_;
  std::vector<std::string> vec_index_;
  std::vector<std::string> vec_cgi_file_extension_;
  bool is_uploadable_;
  std::string upload_dir_;

 private:
  void init();
};

#endif
