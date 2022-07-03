#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <map>
#include <string>

class HttpRequest {
 public:
  HttpRequest() { Init(); }
  ~HttpRequest() {}
  HttpRequest &operator=(const HttpRequest &other) {
    method_ = other.method_;
    uri_ = other.uri_;
    query_string_ = other.query_string_;
    version_ = other.version_;
    host_name_ = other.host_name_;
    content_type_ = other.content_type_;
    content_length_ = other.content_length_;
    body_ = other.body_;
    is_chunked_ = other.is_chunked_;
    is_bad_request_ = other.is_bad_request_;
    header_fields_ = other.header_fields_;
    return *this;
  };

  void Init() {
    method_.clear();
    uri_.clear();
    query_string_.clear();
    version_.clear();
    host_name_.clear();
    content_type_.clear();
    content_length_ = 0;
    body_.clear();
    is_chunked_ = false;
    is_bad_request_ = false;
    header_fields_.clear();
  }

  std::string method_;
  std::string uri_;
  std::string query_string_;
  std::string version_;    // 0以外の数字から始まる文字列
  std::string host_name_;  // Host: からポート番号を取り除いたもの
  std::string content_type_;
  size_t content_length_;
  std::string body_;
  bool is_chunked_;
  // パースする際に、400になるものについてはフラグを立てる
  bool is_bad_request_;
  std::map< std::string, std::string > header_fields_;
};

#endif  // HTTP_REQUEST_HPP_
