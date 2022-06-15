#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <vector>

class HttpRequest {
 public:
  std::string method_;
  std::string uri_;
  std::string http_version_;
  std::string content_type_;
  int content_length_;
  std::string body_;
  bool is_bad_request_;
};

#endif
