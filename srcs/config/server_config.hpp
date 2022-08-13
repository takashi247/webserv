#ifndef SERVER_CONFIG_HPP_
#define SERVER_CONFIG_HPP_

#include <sys/stat.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <netdb.h>   //gethostbyname
#include <arpa/inet.h>  //htonlとか

#include "location_config.hpp"
#include "parser_utils.hpp"

class ServerConfig {
 public:
  ServerConfig();
  ServerConfig(const LocationConfig &lc);  // for debug
  ~ServerConfig();
  ServerConfig(const ServerConfig &rhs);
  ServerConfig &operator=(const ServerConfig &rhs);

  // URIとlocation_path_の前方一致で一番長いものを返す、見つからない場合はNULLは返さず、defaultの値が使われることにする。
  const LocationConfig *SelectLocationConfig(const std::string &uri) const;
  // テスト参照、返したパスにファイルが存在するかどうかは別で確認の必要があり。
  std::string UpdateUri(const std::string &uri) const;
  void PrintVal();
  // other configs are parsed by config utils
  void ParseListen(const std::vector<std::pair<int, std::string> > &list);
  void ValidateServerDuplication(
      const std::vector< ServerConfig > &vec_sc);

  size_t port_;
  in_addr_t host_;
  std::vector< std::string > vec_server_names_;
  std::vector< LocationConfig > vec_location_config_;
  LocationConfig default_location_config_;

 private:
  in_addr_t hostToIp(const std::string &host, int pos);
  std::string addrToStr(in_addr_t addr);
  void Init();
};

#endif
