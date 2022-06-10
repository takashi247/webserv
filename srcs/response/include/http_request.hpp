#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <string>
#include <vector>

struct HttpRequest {
  std::string method_;
  std::string uri_;
  std::string version_;
  std::vector<std::string> header;
  std::vector<std::string> body_;
};

#endif // HTTP_REQUEST_HPP_
