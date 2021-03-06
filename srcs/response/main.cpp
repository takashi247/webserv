#include <iostream>

#include "http_request.hpp"
#include "http_response.hpp"
#include "server_config.hpp"

int main(int ac, char **av) {
  if (ac != 4) {
    return EXIT_FAILURE;
  }
  HttpRequest http_request;
  ServerConfig server_config;
  LocationConfig location_config;
  // server_config.error_page_path_ = "error_page_template.html"; // To test
  // customized error page
  std::string uri = av[2];
  std::string file_type = uri.substr(uri.find_last_of(".") + 1);

  // setting location config paramters
  location_config.vec_cgi_file_extension_.push_back("cgi");
  location_config.vec_accepted_method_.push_back("GET");
  location_config.autoindex_ = true;

  // setting http request config parameters
  http_request.method_ = av[1];
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
  http_request.version_ = av[3];

  // setting server config parameters
  server_config.vec_location_config_.push_back(location_config);

  // http_request.is_bad_request_ = true; // to test 400 response
  HttpResponse http_response(http_request, server_config);
  std::cout << http_response.GetResponse() << std::endl;
}
