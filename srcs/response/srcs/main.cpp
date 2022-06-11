#include <iostream>

#include "http_request.hpp"
#include "server_config.hpp"
#include "http_response.hpp"

int main(int ac, char **av) {
  if (ac != 2) {
    return EXIT_FAILURE;
  }
  HttpRequest http_request;
  ServerConfig server_config;
  std::string uri = av[1];
  std::string file_type = uri.substr(uri.find_last_of(".") + 1);

  http_request.method_ = "GET";
  http_request.uri_ = uri;
  if (file_type == "png") {
    http_request.content_type_ = "image/png";
  } else if (file_type == "jpg") {
    http_request.content_type_ = "image/jpeg";
  } else if (file_type == "ico") {
    http_request.content_type_ = "image/x-icon";
  } else {
    http_request.content_type_ = "text/html";
  }
  HttpResponse http_response(http_request, server_config);
  http_response.MakeResponse();
  std::cout << http_response.GetResponse() << std::endl;
}
