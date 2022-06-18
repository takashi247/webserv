#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>

struct LocationConfig {
  std::string location_path_;
  std::vector< std::string > vec_accepted_method_;
  std::string proxy_pass_;
  std::string root_;
  bool autoindex_;
  std::vector< std::string > vec_index_;
  std::vector< std::string > vec_cgi_file_extension_;
  bool is_uploadable_;
  std::string upload_dir_;
};

class ServerConfig {
 public:
  size_t port_;
  std::string host_;
  std::vector< std::string > server_names_;
  std::string error_page_path_;
  size_t client_max_body_size_;
  std::vector< LocationConfig > vec_location_config_;
};

class Config {
 public:
  Config(const char* config) { (void)config; }
  std::vector< ServerConfig > vec_server_config_;
  std::string config_file_;
  ServerConfig* SelectServerConfig(const std::string& host, const size_t& port,
                                   const std::string& server_name) {
    (void)host;
    (void)port;
    (void)server_name;
    return &vec_server_config_[0];
  }
};
#endif
