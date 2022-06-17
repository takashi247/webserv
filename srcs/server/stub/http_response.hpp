#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>

#include "config.hpp"
#include "http_request.hpp"

class HttpResponse {
 private:
  HttpResponse();
  const HttpRequest &request_;
  const ServerConfig &sc_;

 public:
  HttpResponse(const HttpRequest &http_request,
               const ServerConfig &server_config)
      : request_(http_request), sc_(server_config) {}
  ~HttpResponse() {}
  std::string GetResponse() {
    std::string server_response;
    std::vector< std::string > header;
    header.push_back("HTTP/1.1 200 OK\r\n");
    header.push_back("Content-Type: text/html; charset=UTF-8\r\n");
    header.push_back("Content-Length: 39\r\n");
    header.push_back("Connection: Keep-Alive\r\n");
    header.push_back("\r\n");
    header.push_back("<html><body>Hello,world!</body></html>\r\n");
    int header_size = header.size();
    for (int i = 0; i < header_size; i++) {
      server_response.append(header[i].c_str());
    }
    return server_response;
  }
};

#endif
