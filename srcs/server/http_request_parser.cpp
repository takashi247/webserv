#include "http_request_parser.hpp"

#include <iostream>
#include <istream>  // std::getline
#include <sstream>  // std::stringstream
#include <utility>
#include <vector>

static bool IsToken(const char c) {
  if ('0' <= c && c <= '9') return true;
  if ('A' <= c && c <= 'Z') return true;
  if ('a' <= c && c <= 'z') return true;
  if (c == '!' || c == '#' || c == '$' || c == '%' || c == '&' || c == '\'' ||
      c == '*' || c == '+' || c == '-' || c == '.' || c == '^' || c == '_' ||
      c == '`' || c == '|' || c == '~')
    return true;
  return false;
}

static std::string ChangeLowerCase(const std::string& str) {
  std::string ret;
  std::string::const_iterator it = str.begin();

  while (it != str.end()) {
    if (!IsToken(*it)) return "";
    if ('A' <= *it && *it <= 'Z') {
      ret.push_back(*it + 32);
    } else {
      ret.push_back(*it);
    }
    it++;
  }
  return ret;
}

static bool IsValidFieldContent(const std::string& str) {
  std::string::const_iterator it = str.begin();
  bool isOWS = false;
  while (it != str.end()) {
    if (*it < 0x21 && 0x7e < *it) return false;
    if (*it == ' ' || *it == '\t') {
      if (isOWS)
        return false;
      else
        isOWS = true;
    } else
      isOWS = false;
    it++;
  }
  return true;
}

bool HttpRequestParser::SplitRequestLine(const std::string& recv_msg,
                                         std::vector< std::string >& v) {
  size_t pos = recv_msg.find("\r\n");
  if (pos == std::string::npos) return false;
  std::string req_line = recv_msg.substr(0, pos);
  std::stringstream ss(req_line);
  std::string buf;
  while (std::getline(ss, buf, ' ')) {
    if (!buf.empty()) v.push_back(buf);
  }
  if (v.size() != kReqLineSize) return false;
  // Method
  std::string::iterator it = v[kReqLineMethod].begin();
  for (; it != v[kReqLineMethod].end(); ++it) {
    if (!std::isupper(*it)) return false;
  }
  // Uri
  if ((v[kReqLineUri])[0] != '/') return false;
  // HttpVersion
  if (v[kReqLineHttpVersion].find("HTTP/") != 0)
    return false;
  else if (!isdigit((v[kReqLineHttpVersion])[kProtocolVersionPos]))
    return false;
  else if ((v[kReqLineHttpVersion])[kProtocolVersionPos] == '0')
    return false;
  v[kReqLineHttpVersion].erase(0, kProtocolVersionPos);

  return true;
}

/*
 * 0: ??????????????????bad request
 * other: ??????????????????????????????
 */
int HttpRequestParser::GetHeaderFields(
    const std::string& recv_msg, std::map< std::string, std::string >* fields) {
  size_t cur = recv_msg.find("\r\n") + 2;
  size_t start = cur;

  while (start != recv_msg.find("\r\n", start)) {
    cur = recv_msg.find(":", start);
    std::string key = recv_msg.substr(start, cur - start);
    key = ChangeLowerCase(key);
    if (key.empty()) return 0;
    cur++;
    while (recv_msg[cur] == ' ' || recv_msg[cur] == '\t') cur++;
    start = cur;
    cur = recv_msg.find("\r\n", start);
    while (recv_msg[cur - 1] == ' ' || recv_msg[cur - 1] == '\t') cur--;
    // ' ' ??? '\t'??????????????????????????????
    std::string value = recv_msg.substr(start, cur - start);
    if (!IsValidFieldContent(value)) {
      std::cout << "is not valid field content\n";
      return 0;
    }
    while (recv_msg[cur] == ' ' || recv_msg[cur] == '\t') cur++;
    if (recv_msg.find("\r\n", cur) != cur) {
      std::cout << "unexpected header field\n";
      return 0;
    }
    fields->insert(std::make_pair(key, value));
    start = cur + 2;
  }
  return start + 2;  // body??????????????????
}

int HttpRequestParser::ParseHeader(const std::string& recv_msg,
                                   HttpRequest* req) {
  std::vector< std::string > req_line;
  if ((req->is_bad_request_ = !SplitRequestLine(recv_msg, req_line))) {
    return 1;
  }
  req->method_ = req_line[kReqLineMethod];
  {
    std::string uri = req_line[kReqLineUri];
    size_t start_pos = uri.find('?');
    if (start_pos == std::string::npos)
      req->query_string_ = "";
    else {  // ???????????????????????????
      req->query_string_ = uri.substr(start_pos + 1);
      uri = uri.substr(0, start_pos);
    }
    req->uri_ = uri;
  }
  req->version_ = req_line[kReqLineHttpVersion];

  size_t pos;
  pos = GetHeaderFields(recv_msg, &req->header_fields_);
  if (pos == 0) {
    req->is_bad_request_ = true;
    return 1;
  }

  std::string host = req->header_fields_["host"];
  if (host.empty()) {
    req->is_bad_request_ = true;
  } else {  // separate host -> name:port
    pos = host.find(":");
    if (pos == std::string::npos)
      req->host_name_ = host;
    else
      req->host_name_ = host.substr(0, pos);
  }
  req->content_type_ = req->header_fields_["content-type"];
  std::stringstream sstream(req->header_fields_["content-length"]);
  sstream >> req->content_length_;
  {  // judge encoding
    std::string encoding = req->header_fields_["Transfer-Encoding"];
    req->is_chunked_ = (encoding == "chunked");
  }
  return 0;
}
