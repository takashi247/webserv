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
#include <cctype> // for isdigit
#include <sys/stat.h>

class HttpResponse {
 public:
  // Constructor
  HttpResponse(const HttpRequest &http_request, const ServerConfig &server_config);

  // Destructor
  virtual ~HttpResponse();

  // Other functions
  std::string GetResponse() const;

 private:
  // Static constants
  static const int kStatusCodeOK = 200;
  static const int kStatusCodeBadRequest = 400;
  static const int kStatusCodeForbidden = 403;
  static const int kStatusCodeNotFound = 404;
  static const int kStatusCodeMethodNotAllowed = 405;
  static const int kStatusCodeVersionNotSupported = 505;
  static const std::string kServerVersion;
  static const std::string kStatusDescOK;
  static const std::string kStatusDescBadRequest;
  static const std::string kStatusDescForbidden;
  static const std::string kStatusDescNotFound;
  static const std::string kStatusDescMethodNotAllowed;
  static const std::string kStatusDescVersionNotSupported;

  // Constructor and assignment operators
  HttpResponse();
  HttpResponse(const HttpResponse &other);
  HttpResponse &operator=(const HttpResponse &other);

  // Other functions
  void InitParameters();
  void MakeResponse();
  void InitFileStream();
  void MakeHeader200();
  void MakeErrorHeader();
  void MakeBody200();
  void MakeErrorBody();
  void CreateDefaultErrorPage();
  void CreateCustomizedErrorPage();
  void PrintErrorMessage(const std::string &msg) const;
  void SetCurrentTime();
  void SetLastModifiedTime();
  void SetStatusCode();
  bool IsValidMethod() const;
  bool IsAllowedMethod() const;
  void ValidateVersion();
  void SetEtag();
  bool IsDigitSafe(char ch);
  void SetContentType();

  // Data members
  HttpRequest http_request_;
  ServerConfig server_config_;
  int status_code_;
  std::string status_desc_;
  bool is_bad_request_;
  bool is_supported_version_;
  LocationConfig* location_config_;
  std::string requested_file_path_;
  std::ifstream requested_file_;
  bool is_file_exists_;
  bool is_file_forbidden_;
  std::string server_header_;
  std::string date_header_;
  std::string etag_header_;
  std::vector<std::string> header_;
  std::string body_;
  size_t body_len_;
  std::string response_;
  std::string current_time_;
  std::string last_modified_;
  std::string content_type_;
};

#endif // HTTP_RESPONSE_HPP_
