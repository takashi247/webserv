#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <cstring>
#include <cstdlib>
#include "http_request.hpp"

class HttpRequestParser {
 private:
  static const int kProtocolVersionPos = 5;

  static std::string GetMethod(const std::string& recv_msg);
  static std::string GetUri(const std::string& recv_msg);
  static std::string GetProtocolVersion(const std::string& recv_msg);
  static bool IsValidHttpVersion(const std::string& recv_msg);
  static std::string GetFieldValue(const char* field_name,
                                   const std::string& recv_msg);
  static size_t GetFieldValueSize(const char* field_name,
                                  const std::string& recv_msg);
  static void GetMessageBody(const std::string& recv_msg, std::string& body);

 public:
  static int Parse(const std::string& recv_msg, HttpRequest* request);
};

#endif
