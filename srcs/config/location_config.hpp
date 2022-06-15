#ifndef LOCATIONCONFIG_HPP_
#define LOCATIONCONFIG_HPP_

#include <set>
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
  void PrintVal();

  int ParseLocation(std::stringstream &ss);

 private:
  void init();
  int ParseDirective(std::string directive);
};

#endif
