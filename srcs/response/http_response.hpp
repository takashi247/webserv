#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include <dirent.h>    // for opendir, readdir
#include <string.h>    // for strdup
#include <sys/stat.h>  // for stat
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>  // for sort
#include <cctype>     // for isdigit
#include <cstring>    // for debugging
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "http_request.hpp"
#include "server_config.hpp"

class HttpResponse {
 public:
  // Constructor
  HttpResponse(const HttpRequest &http_request,
               const ServerConfig &server_config);

  // Destructor
  virtual ~HttpResponse();

  // Other functions
  std::string GetResponse() const;
  static std::map< std::string, std::string > CreateMimeTypeMap();

 private:
  // Static constants
  static const int kStatusCodeOK = 200;
  static const int kStatusCodeNoContent = 204;
  static const int kStatusCodeMovedPermanently = 301;
  static const int kStatusCodeBadRequest = 400;
  static const int kStatusCodeForbidden = 403;
  static const int kStatusCodeNotFound = 404;
  static const int kStatusCodeMethodNotAllowed = 405;
  static const int kStatusCodeRequestEntityTooLarge = 413;
  static const int kStatusCodeVersionNotSupported = 505;
  static const int kCgiBufferSize = 500;
  static const int kAsciiCodeForEOF = 26;
  static const std::string kServerVersion;
  static const std::string kStatusDescOK;
  static const std::string kStatusDescNoContent;
  static const std::string kStatusDescMovedPermanently;
  static const std::string kStatusDescBadRequest;
  static const std::string kStatusDescForbidden;
  static const std::string kStatusDescNotFound;
  static const std::string kStatusDescMethodNotAllowed;
  static const std::string kStatusDescRequestEntityTooLarge;
  static const std::string kStatusDescVersionNotSupported;
  static const std::map< std::string, std::string > kMimeTypeMap;

  // Constructor and assignment operators
  HttpResponse();
  HttpResponse(const HttpResponse &other);
  HttpResponse &operator=(const HttpResponse &other);

  // Other functions
  void InitParameters();
  void MakeResponse();
  void InitFileStream();
  void MakeHeader200();
  void MakeHeader204();
  void MakeHeader301();
  void MakeErrorHeader();
  void MakeBody200();
  void MakeBody301();
  void DeleteRequestedFile();
  void MakeErrorBody();
  void CreateDefaultErrorPage();
  void CreateCustomizedErrorPage(const std::string &error_page_path);
  void PrintErrorMessage(const std::string &msg) const;
  void SetCurrentTime();
  void SetLastModifiedTime(const std::string &path);
  void SetStatusCode();
  bool IsValidMethod() const;
  bool IsAllowedMethod() const;
  void ValidateVersion();
  void SetEtag();
  bool IsDigitSafe(char ch);
  void SetContentType();
  bool IsCgiFileExtension(const std::string &file_type) const;
  void MakeCgiBody();
  void MakeCgiHeader();
  void ValidatePath();
  void CreateAutoindexPage();
  std::vector< std::string > GetFileNames();
  std::string CreateFileList(std::vector< std::string >);
  char **CreateCgiEnviron();
  void DeleteCgiEnviron(char **cgi_env);
  bool IsDirectory(const std::string &path) const;
  void CheckRedirection();
  void RemoveIndex(std::string &modified_path);

  // Data members
  const HttpRequest &http_request_;
  const ServerConfig &server_config_;
  int status_code_;
  std::string status_desc_;
  bool is_bad_request_;
  bool is_supported_version_;
  std::string content_type_;
  std::string requested_file_path_;
  const LocationConfig *location_config_;
  std::ifstream requested_file_;
  bool is_file_exists_;
  bool is_file_forbidden_;
  bool is_path_exists_;
  bool is_path_forbidden_;
  bool is_redirected_;
  std::string server_header_;
  std::string date_header_;
  std::string etag_header_;
  std::vector< std::string > header_;
  std::string body_;
  size_t body_len_;
  std::string response_;
  std::string current_time_;
  std::string last_modified_;
  std::string file_type_;
};

#endif  // HTTP_RESPONSE_HPP_
