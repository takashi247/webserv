#include "http_response.hpp"

HttpResponse::HttpResponse(const HttpRequest &http_request, const ServerConfig &server_config)
  : http_request_(http_request), server_config_(server_config) {}

HttpResponse::~HttpResponse() {}

int HttpResponse::MakeResponse() {
  response_ = "Hello world!";
  return 0;
}

const char* HttpResponse::GetResponse() const {
  return response_.c_str();
}
