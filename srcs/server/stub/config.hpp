#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>

class LocationConfig {
 public:
  LocationConfig(){};
  ~LocationConfig(){};
  //  LocationConfig(const LocationConfig& rhs){};
  //  LocationConfig& operator=(const LocationConfig& rhs);

  void PrintVal();

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
  ServerConfig(){};
  ServerConfig(const LocationConfig& lc) { (void)lc; };  // for debug
  ~ServerConfig(){};
  ServerConfig(const ServerConfig& rhs) { *this = rhs; };
  ServerConfig& operator=(const ServerConfig& rhs) {
    this->port_ = rhs.port_;
    this->vec_location_config_ = rhs.vec_location_config_;
    this->vec_server_names_ = rhs.vec_server_names_;
    this->host_ = rhs.host_;
    return *this;
  };

  // URIとlocation_path_の前方一致で一番長いものを返す、見つからない場合はNULLは返さず、defaultの値が使われることにする。
  // LocationConfig* SelectLocationConfig(const std::string& uri) {
  //   (void)uri;
  //   return NULL;
  // };
  // //
  // テスト参照、返したパスにファイルが存在するかどうかは別で確認の必要があり。
  // std::string UpdateUri(std::string uri) {
  //   (void)uri;
  //   return "";
  // };
  // void PrintVal(){};
  // // other configs are parsed by config utils
  // void ParseListen(const std::vector< std::pair< int, std::string > >& list)
  // {
  //   (void)list;
  // };

  size_t port_;
  std::string host_;
  std::vector< std::string > vec_server_names_;
  std::string error_page_path_;
  size_t client_max_body_size_;
  std::vector< LocationConfig > vec_location_config_;
  LocationConfig default_location_config_;

 private:
  void Init();
};

class Config {
 public:
  Config(const char* config) { (void)config; }
  std::vector< ServerConfig > vec_server_config_;
  std::string config_file_;
  ServerConfig* SelectServerConfig(const std::string& host,
                                   const size_t& port) {
    (void)host;
    (void)port;
    return &vec_server_config_[0];
  }
};
#endif
