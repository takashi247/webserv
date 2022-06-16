#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#ifdef DUMMY_RESPONSE
#include "http_request.hpp"
#else
#include "../response/include/http_request.hpp"
#endif

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
  static HttpRequest* CreateHttpRequest(const std::string& recv_msg);
  static void DestroyHttpRequest(HttpRequest* req);
};

#endif
