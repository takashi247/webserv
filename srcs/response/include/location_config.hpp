#ifndef LOCATION_CONFIG_HPP_
#define LOCATION_CONFIG_HPP_

#include <string>
#include <vector>

struct LocationConfig {
  std::vector<std::string> vec_accepted_method_;
  std::string proxy_path_; // @fyuta: proxy_pathについては複数個設定可能かもしれないので、その場合はstd::vector<std::string>に変更か
  std::string root_;
  bool autoindex_;
  std::vector<std::string> vec_index_;
  std::vector<std::string> vec_cgi_file_extension_;
  bool is_uploadable_;
  std::string upload_dir_;
};

#endif // LOCATION_CONFIG_HPP_
