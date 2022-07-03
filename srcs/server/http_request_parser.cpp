#include "http_request_parser.hpp"

#include <iostream>
#include <istream>  // std::getline
#include <sstream>  // std::stringstream
#include <utility>
#include <vector>

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

std::string HttpRequestParser::GetFieldValue(const char* field_name,
                                             const std::string& recv_msg) {
  std::string search_name(CRLF);
  search_name.append(field_name);  // "\r\n"field の文字列を生成
  size_t pos = recv_msg.find(search_name, 0);  // 探す
  if (std::string::npos == pos) return "";  // field_nameがみつから無ければ終了
  pos += (kCRLFSize + strlen(field_name));
  if (recv_msg[pos] != ':') return "";  // field_nameの直後に : でなければ終了
  pos++;                                // : の直後を指す
  pos = recv_msg.find_first_not_of(' ', pos);  // スペース読み飛ばす
  //  std::cout << "pos=" << pos << std::endl;
  if (0 == recv_msg.find(CRLF, pos)) return "";  // field_name:  \r\nなら終了
  //この時点でposは値の先頭
  //  std::cout << "find CRLF OK" << std::endl;
  size_t value_end_pos = recv_msg.find("\r\n", pos);
  std::string value(recv_msg.begin() + pos, recv_msg.begin() + value_end_pos);
  return value;
}

size_t HttpRequestParser::GetFieldValueSize(const char* field_name,
                                            const std::string& recv_msg) {
  std::string value = GetFieldValue(field_name, recv_msg);
  int num = atoi(value.c_str());
  return num;
}

int HttpRequestParser::GetHeaderFields(
    const std::string& recv_msg, std::map< std::string, std::string >* fields) {
  size_t cur = recv_msg.find("\r\n") + 2;
  size_t start = cur;

  while (start != recv_msg.find("\r\n", start)) {
    cur = recv_msg.find(":", start);
    cur = recv_msg.find_first_not_of(' ', cur);
    std::string key = recv_msg.substr(start, cur - start);
    start = cur + 2;
    cur = recv_msg.find("\r\n", start);
    std::string value = recv_msg.substr(start, cur - start);
    start = cur + 2;
    fields->insert(std::make_pair(key, value));
  }
  return start + 2;  // bodyの先頭を指す
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
    else {  // クエリーがある場合
      req->query_string_ = uri.substr(start_pos, uri.size() - (start_pos + 1));
      uri = uri.substr(0, start_pos);
    }
    req->uri_ = uri;
  }
  req->version_ = req_line[kReqLineHttpVersion];

  size_t pos;
  std::string host = GetFieldValue("Host", recv_msg);
  if (host.empty()) {
    req->is_bad_request_ = true;
  } else {  // separate host -> name:port
    pos = host.find(":");
    if (pos == std::string::npos)
      req->host_name_ = host;
    else
      req->host_name_ = host.substr(0, pos);
  }
  req->content_type_ = GetFieldValue("Content-Type", recv_msg);
  req->content_length_ = GetFieldValueSize("Content-Length", recv_msg);
  {  // judge encoding
    std::string encoding = GetFieldValue("Transfer-Encoding", recv_msg);
    req->is_chunked_ = (encoding == "chunked");
  }
  pos = GetHeaderFields(recv_msg, &req->header_fields_);

  if (0) {
    std::cout << req->method_ << std::endl;
    std::cout << req->uri_ << std::endl;
    std::cout << req->version_ << std::endl;
    std::cout << req->host_name_ << std::endl;
    std::cout << req->content_type_ << std::endl;
    std::cout << req->content_length_ << std::endl;
    std::cout << req->body_ << std::endl;
  }
  return 0;
}
