#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <vector>

struct HttpRequest {
  std::string method_;
  std::string uri_;
  std::string version_;
  std::string host_name_;
  std::string host_port_;
  std::string content_type_;
  size_t content_length_;
  std::string body_;

  bool is_bad_request_;
};

#endif
