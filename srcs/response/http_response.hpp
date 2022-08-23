#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include <dirent.h>    // for opendir, readdir, closedir
#include <fcntl.h>     // for open
#include <stdlib.h>    // for free
#include <string.h>    // for strdup
#include <sys/stat.h>  // for stat
#include <unistd.h>    // for alarm

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

#include "client_info.hpp"
#include "http_request.hpp"
#include "server_config.hpp"
#include "wrapper.hpp"

class HttpResponse {
 public:
  // Constructor
  HttpResponse();

  // HttpResponse(const HttpRequest &http_request,
  //              const ServerConfig &server_config,
  //              const t_client_info &client_info);

  // Destructor
  virtual ~HttpResponse();

  // Other functions
  std::string GetResponse() const;
  std::string GetConnection() const;
  static std::map< std::string, std::string > CreateMimeTypeMap();
  void Init(HttpRequest *http_request, const ServerConfig *server_config,
            t_client_info *client_info);
  int Create(bool is_readable, bool is_cgi_readable, bool is_cgi_writable);
  void Reset();
  int GetResponseReadFd() const;
  int GetCgiReadFd() const;
  int GetCgiWriteFd() const;

 private:
  // Static constants
  static const int kStatusCodeOK = 200;
  static const int kStatusCodeNoContent = 204;
  static const int kStatusCodeMovedPermanently = 301;
  static const int kStatusCodeFound = 302;
  static const int kStatusCodeBadRequest = 400;
  static const int kStatusCodeForbidden = 403;
  static const int kStatusCodeNotFound = 404;
  static const int kStatusCodeMethodNotAllowed = 405;
  static const int kStatusCodeRequestEntityTooLarge = 413;
  static const int kStatusCodeInternalServerError = 500;
  static const int kStatusCodeMethodNotImplemented = 501;
  static const int kStatusCodeGatewayTimeout = 504;
  static const int kStatusCodeVersionNotSupported = 505;
  static const int kReadBufferSize = 500;
  static const int kAsciiCodeForEOF = 26;
  static const size_t kLenOfStatusCode = 3;
  static const size_t kLenOfDateBuffer = 100;
  static const int kFdNotSet = -1;
  static const int kReadFd = 0;
  static const int kWriteFd = 1;
  static const std::string kServerVersion;
  static const std::string kStatusDescOK;
  static const std::string kStatusDescNoContent;
  static const std::string kStatusDescMovedPermanently;
  static const std::string kStatusDescFound;
  static const std::string kStatusDescBadRequest;
  static const std::string kStatusDescForbidden;
  static const std::string kStatusDescNotFound;
  static const std::string kStatusDescMethodNotAllowed;
  static const std::string kStatusDescRequestEntityTooLarge;
  static const std::string kStatusDescInternalServerError;
  static const std::string kStatusDescMethodNotImplemented;
  static const std::string kStatusDescGatewayTimeout;
  static const std::string kStatusDescVersionNotSupported;
  static const std::string kConnectionKeepAlive;
  static const std::string kConnectionClose;
  static const std::map< std::string, std::string > kMimeTypeMap;
  static const unsigned int kCgiTimeout = 30;

  // Constructor and assignment operators
  HttpResponse(const HttpResponse &other);
  HttpResponse &operator=(const HttpResponse &other);

  // Other functions
  void InitParameters();
  void MakeResponse();
  void InitFileStream();
  void MakeHeader200();
  void MakeHeader204();
  void MakeHeaderRedirection();
  void MakeErrorHeader();
  void MakeBody200();
  void MakeBodyRedirection();
  void DeleteRequestedFile();
  void MakeErrorBody();
  void CreateDefaultErrorPage();
  int OpenCustomizedErrorPage(const std::string &error_page_path);
  void CreateResponseBody();
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
  // void MakeCgiResponse();
  void CreateCgiHeader();
  void ValidatePath();
  void CreateAutoindexPage();
  std::vector< std::string > GetFileNames();
  std::string CreateFileList(std::vector< std::string >);
  char **CreateCgiEnviron();
  void DeleteCgiEnviron(char **cgi_env);
  bool IsDirectory(const std::string &path) const;
  void RemoveIndex(std::string &modified_path);
  std::string GetHeaderValue(const std::string &header_name);
  void ExtractPathInfo();
  void ParseCgiHeader();
  int ExtractStatusCode(const std::string &header_value);
  void SetStatusDescription();
  std::string ExtractLocationPath(const std::string &header_value);
  std::string GetFieldValue(const std::string &header_field);
  bool CreateCgiBody(bool has_content_length, bool is_read_finish);
  void Make500Response();
  void Make504Response();
  bool IsRequestConnectionClose() const;
  int SendRequestBody(int fd, const HttpRequest &http_request) const;
  bool IsImplementedMethod() const;
  bool IsCgiRequest() const;
  void PrepareCgiResponse();
  void WriteRequestBody();
  void ReadCgiOutput();
  ssize_t ReadFdIntoBody(int fd);
  int CloseNResetFd(int &fd);

  // Helper functions
  std::string ShortenRequestBody(const std::string &);

  template < class _Integer >
  std::string IntegerToString(_Integer num) {
    std::stringstream ss;
    ss << num;
    return ss.str();
  }

  template < class _Integer >
  _Integer StringToInteger(std::string num_str) {
    _Integer num;
    std::stringstream ss;
    ss << num_str;
    ss >> num;
    return num;
  }

  // Data members
  typedef enum e_response_status {
    kInitResponse = 0,
    kOpenFile,
    kCreateResponseBody,
    kCreateResponseHeader,
    kWriteRequestBody,
    kReadCgiOutput,
    kResponseIsReady
  } t_response_status;

  typedef enum e_cgi_status {
    kReadHeader = 0,
    kParseHeader,
    kCreateBody,
    kCloseConnection
  } t_cgi_status;

  HttpRequest *http_request_;
  const ServerConfig *server_config_;
  t_client_info *client_info_;
  int status_code_;
  std::string status_desc_;
  t_response_status response_status_;
  t_cgi_status cgi_status_;
  int fd_response_read_;
  int fd_cgi_read_;
  int fd_cgi_write_;
  bool is_bad_request_;
  bool is_supported_version_;
  bool is_file_exists_;
  bool is_path_exists_;
  bool is_file_forbidden_;
  bool is_path_forbidden_;
  bool is_permanently_redirected_;
  bool is_temporarily_redirected_;
  std::string content_type_;
  std::string original_uri_;
  bool has_content_length_header_;
  bool is_local_redirection_;
  std::string requested_file_path_;
  const LocationConfig *location_config_;
  std::string path_info_;
  std::string path_translated_;
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
  std::string connection_;
  int pipe_child2parent_[2];
  int pipe_parent2child_[2];
  pid_t cgi_pid_;
};

#endif  // HTTP_RESPONSE_HPP_
