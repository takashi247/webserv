#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include "http_request.hpp"
#include "server_config.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/stat.h>

class HttpResponse {
 public:
  HttpResponse(const HttpRequest &http_request, const ServerConfig &server_config);
  virtual ~HttpResponse();
  int MakeResponse();
  const char* GetResponse() const;
  size_t GetResponseLen() const;

 private:
  HttpResponse();
  HttpResponse(const HttpResponse &other);
  HttpResponse &operator=(const HttpResponse &other);
  bool IsValidRequest();
  int MakeResponseLine();
  int MakeHeader();
  int MakeBody();
  void MakeHeader200();
  void MakeHeader404();
  void PrintErrorMessage(const std::string &msg) const;
  void SetCurrentTime();
  void SetLastModifiedTime();

  HttpRequest http_request_;
  ServerConfig server_config_;
  std::vector<std::string> header_;
  std::string body_;
  size_t body_len_;
  std::string response_;
  bool is_file_fail_;
  std::string current_time_;
  std::string last_modified_;
};

#endif // HTTP_RESPONSE_HPP_
