#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <string>
#include <vector>

class HttpRequest {
 public:
  std::string method_;
  std::string uri_;
  std::string version_; // @skohraku: 0以外の数字から始まる文字列が来ることを想定しています
  std::string content_type_;
  std::vector<std::string> header;
  std::vector<std::string> body_;
  bool is_bad_request_; // @skohraku: パースする際に、400になるものについてはフラグを立てて頂けるとありがたいです。
};

#endif // HTTP_REQUEST_HPP_
