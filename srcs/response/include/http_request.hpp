#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <string>
#include <vector>

class HttpRequest {
 public:
  std::string method_;
  std::string uri_;
  std::string version_;
  std::string content_type_;
  std::vector<std::string> header;
  std::vector<std::string> body_;
};

#endif // HTTP_REQUEST_HPP_
