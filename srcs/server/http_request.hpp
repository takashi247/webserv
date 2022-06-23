#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <string>
#include <vector>

struct HttpRequest {
  std::string method_;
  std::string uri_;
  std::string version_;  // 0以外の数字から始まる文字列
  std::string host_name_;
  size_t host_port_;
  std::string content_type_;
  size_t content_length_;
  std::string body_;
  bool is_chunked_;
  bool
      is_bad_request_;  // パースする際に、400になるものについてはフラグを立てる
};

#endif  // HTTP_REQUEST_HPP_
