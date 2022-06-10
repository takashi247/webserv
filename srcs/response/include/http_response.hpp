#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include "http_request.hpp"
#include "server_config.hpp"

#include <string>
#include <vector>

class HttpResponse {
 public:
  HttpResponse(const HttpRequest &http_request, const ServerConfig &server_config);
  virtual ~HttpResponse();
  int MakeResponseLine();
  int MakeHeader();
  int MakeBody();
  int MakeResponse();
  const char* GetResponse() const;
  size_t GetResponseLen();

 private:
  HttpResponse();
  HttpResponse(const HttpResponse &other);
  HttpResponse &operator=(const HttpResponse &other);

  HttpRequest http_request_;
  ServerConfig server_config_;
  std::string response_line_;
  std::vector<std::string> header_;
  std::vector<std::string> body_;
  std::string response_;
};

#endif // HTTP_RESPONSE_HPP_
