#ifndef SERVER_CONFIG_HPP_
#define SERVER_CONFIG_HPP_

#include "location_config.hpp"

#include <string>
#include <vector>

class ServerConfig {
 public:
  // @fyuta: uriを渡すとroot/location configをベースにproxyとかをたどって本当に開くべきファイルパスを返してくれる関数を実装してもらえませんでしょうか
  std::string UpdateUri(std::string uri);

  /*
  ** @fyuta: 下記の関数を実装して頂けませんでしょうか。
  ** 引数：uri
  ** 戻り値：みつかったlocation configのポインタ、見つからなければNULL
  */
  LocationConfig* SelectLocationConfig(const std::string& uri);

  size_t port_; // The highest TCP port number is 65535
  std::string host_;
  std::vector<std::string> server_names_;
  std::string error_page_path_;
  size_t client_max_body_size_; // @fyuta: ConfigParserの方で数字に読み替えてもらえるとありがたいです。intで収まり切るかどうかは要検討
  std::vector<LocationConfig> vec_location_config_;
};

#endif // SERVER_CONFIG_HPP_
