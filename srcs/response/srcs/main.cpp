#include <iostream>

#include "http_request.hpp"
#include "server_config.hpp"
#include "http_response.hpp"

int main(void) {
  HttpRequest http_request;
  ServerConfig server_config;

  HttpResponse http_response(http_request, server_config);
  http_response.MakeResponse();
  std::cout << http_response.GetResponse() << std::endl;
}
