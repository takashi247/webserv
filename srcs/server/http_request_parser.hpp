#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <cstdlib>
#include <cstring>

#include "http_request.hpp"

#define SEPARATOR "\r\n\r\n"

class HttpRequestParser {
 private:
  static const int kProtocolVersionPos = 5;
  static const int kSeparatorSize = 4;
  static const int kCRLFSize = 2;

  static std::string GetMethod(const std::string& recv_msg);
  static std::string GetUri(const std::string& recv_msg);
  static std::string GetProtocolVersion(const std::string& recv_msg);
  static bool IsValidHttpVersion(const std::string& recv_msg);
  static std::string GetFieldValue(const char* field_name,
                                   const std::string& recv_msg);
  static size_t GetFieldValueSize(const char* field_name,
                                  const std::string& recv_msg);
  static int GetChunkSize(std::string::const_iterator* it);

 public:
  static void GetMessageBody(const std::string& recv_msg, bool is_chunked,
                             std::string& body);

  static int Parse(const std::string& recv_msg, HttpRequest* request);
};

#endif
