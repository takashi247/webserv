#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <cstdlib>
#include <cstring>
#include <utility>

#include "http_request.hpp"

#define SEPARATOR "\r\n\r\n"

class HttpRequestParser {
 public:
  static const int kProtocolVersionPos = 5;
  static const int kSeparatorSize = 4;
  static const int kCRLFSize = 2;

 private:
  static std::string GetMethod(const std::string& recv_msg);
  static std::pair< std::string, std::string > GetUri(
      const std::string& recv_msg);
  static std::string GetProtocolVersion(const std::string& recv_msg);
  static bool IsValidHttpVersion(const std::string& recv_msg);
  static std::string GetFieldValue(const char* field_name,
                                   const std::string& recv_msg);
  static size_t GetFieldValueSize(const char* field_name,
                                  const std::string& recv_msg);
  static int GetHeaderFields(const std::string& recv_msg,
                             std::map< std::string, std::string >* fields);

 public:
  static int ParseHeader(const std::string& recv_msg, HttpRequest* request);
};

#endif
