#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <cstdlib>
#include <cstring>
#include <utility>
#include <vector>

#include "http_request.hpp"

#define CRLF "\r\n"
#define SEPARATOR "\r\n\r\n"

class HttpRequestParser {
 public:
  static const int kProtocolVersionPos = 5;
  static const int kSeparatorSize = 4;
  static const int kCRLFSize = 2;

  enum e_req_line {
    kReqLineMethod = 0,
    kReqLineUri,
    kReqLineHttpVersion,
    kReqLineSize
  };

 private:
  static bool SplitRequestLine(const std::string& recv_msg,
                               std::vector< std::string >& v);
  static int GetHeaderFields(const std::string& recv_msg,
                             std::map< std::string, std::string >* fields);

 public:
  static int ParseHeader(const std::string& recv_msg, HttpRequest* request);
};

#endif
