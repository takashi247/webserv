#include "http_request_parser.hpp"

#include <iostream>
#include <utility>

std::string HttpRequestParser::GetMethod(const std::string& recv_msg) {
  size_t pos = recv_msg.find(' ');
  std::string method(recv_msg.begin(), recv_msg.begin() + pos);
  return method;
}
std::pair< std::string, std::string > HttpRequestParser::GetUri(
    const std::string& recv_msg) {
  size_t start_pos = recv_msg.find(' ');
  size_t end_pos = recv_msg.find(' ', start_pos + 1);
  std::string uri(recv_msg.begin() + start_pos + 1, recv_msg.begin() + end_pos);
  start_pos = uri.find('?');
  if (start_pos == std::string::npos) return std::make_pair(uri, "");
  // クエリーがある場合
  start_pos++;
  std::string query(uri.substr(start_pos, uri.size() - start_pos));
  uri = uri.substr(0, start_pos - 1);
  return std::make_pair(uri, query);
}
std::string HttpRequestParser::GetProtocolVersion(const std::string& recv_msg) {
  size_t start_pos = recv_msg.find(' ');
  start_pos = recv_msg.find(' ', start_pos + 1);
  size_t end_pos = recv_msg.find("\r\n", start_pos + 1);
  std::string version(recv_msg.begin() + start_pos + 1,
                      recv_msg.begin() + end_pos);
  return version;
}
bool HttpRequestParser::IsValidHttpVersion(
    const std::string& protocol_version) {
  size_t pos = protocol_version.find("HTTP/");
  char version_head_num = protocol_version[kProtocolVersionPos];
  if (pos == 0 && ('0' < version_head_num && version_head_num <= '9'))
    return true;
  return false;
}
std::string HttpRequestParser::GetFieldValue(const char* field_name,
                                             const std::string& recv_msg) {
  size_t name_start_pos = recv_msg.find(field_name, 0, strlen(field_name));
  if (std::string::npos == name_start_pos) return "";
  size_t value_start_pos = recv_msg.find(": ", name_start_pos + 1) + 2;
  size_t value_end_pos = recv_msg.find("\r\n", value_start_pos + 1);
  std::string value(recv_msg.begin() + value_start_pos,
                    recv_msg.begin() + value_end_pos);
  return value;
}
size_t HttpRequestParser::GetFieldValueSize(const char* field_name,
                                            const std::string& recv_msg) {
  std::string value = GetFieldValue(field_name, recv_msg);
  int num = atoi(value.c_str());
  return num;
}
int HttpRequestParser::GetChunkSize(std::string::const_iterator* it) {
  std::string::const_iterator cur = *it;
  int size = 0;
  while (('0' <= *cur && *cur <= '9') || ('A' <= *cur && *cur <= 'F')) {
    size *= 16;
    if ('0' <= *cur && *cur <= '9')
      size += *cur - '0';
    else if ('A' <= *cur && *cur <= 'F')
      size += *cur - 'A' + 10;
    ++cur;
  }
  if (*cur != '\r' || *(cur + 1) != '\n') {
    std::cout << "Unexpected Separator" << std::endl;
    return 0;
  }
  *it = cur + kCRLFSize;
  return size;
}
void HttpRequestParser::GetMessageBody(const std::string& recv_msg,
                                       bool is_chunked, std::string& body) {
  size_t cur = recv_msg.find(SEPARATOR) + kSeparatorSize;
  if (!is_chunked) {
    body.assign(recv_msg.begin() + cur, recv_msg.end());
    return;
  }

  std::string::const_iterator it = recv_msg.begin() + cur;
  int size = GetChunkSize(&it);
  while (0 < size) {
    body.append(&(*it), size);
    it += size;
    if (*it != '\r' || *(it + 1) != '\n') {
      std::cout << "Unexpected Separator" << std::endl;
      break;
    }
    it += kCRLFSize;
    size = GetChunkSize(&it);
  }
  // std::cout << "size=" << body.size() << " [" << body << "]" << std::endl;
  return;
}

int HttpRequestParser::GetHeaderFields(
    const std::string& recv_msg, std::map< std::string, std::string >* fields) {
  size_t cur = recv_msg.find("\r\n") + 2;
  size_t start = cur;

  while (start != recv_msg.find("\r\n", start)) {
    cur = recv_msg.find(": ", start);
    std::string key = recv_msg.substr(start, cur - start);
    start = cur + 2;
    cur = recv_msg.find("\r\n", start);
    std::string value = recv_msg.substr(start, cur - start);
    start = cur + 2;
    fields->insert(std::make_pair(key, value));
  }
  return start + 2;  // bodyの先頭を指す
}

int HttpRequestParser::Parse(const std::string& recv_msg, HttpRequest* req) {
  req->is_bad_request_ = false;
  req->method_ = GetMethod(recv_msg);
  std::pair< std::string, std::string > uri = GetUri(recv_msg);
  req->uri_ = uri.first;
  req->query_string_ = uri.second;
  {  // Http version check
    std::string version = GetProtocolVersion(recv_msg);
    bool is_valid_version = IsValidHttpVersion(version);
    if (!is_valid_version) {
      req->is_bad_request_ = true;
      return (1);
    }
    req->version_ =
        version.erase(0, kProtocolVersionPos);  //先頭の"HTTP/"を削除
  }
  size_t pos;
  std::string host = GetFieldValue("Host", recv_msg);
  {  // separate host -> name:port
    pos = host.find(":");
    if (pos == std::string::npos)
      req->host_name_ = host;
    else {
      req->host_name_ = host.substr(0, pos++);
      req->host_port_ = atoi(host.substr(pos, host.size()).c_str());
    }
  }
  req->content_type_ = GetFieldValue("Content-Type", recv_msg);
  req->content_length_ = GetFieldValueSize("Content-Length", recv_msg);
  {  // judge encoding
    std::string encoding = GetFieldValue("Transfer-Encoding", recv_msg);
    req->is_chunked_ = (encoding == "chunked");
  }
  pos = GetHeaderFields(recv_msg, &req->header_fields_);
  GetMessageBody(recv_msg, req->is_chunked_, req->body_);

  if (0) {
    std::cout << req->method_ << std::endl;
    std::cout << req->uri_ << std::endl;
    std::cout << req->version_ << std::endl;
    std::cout << req->host_name_ << std::endl;
    std::cout << req->host_port_ << std::endl;
    std::cout << req->content_type_ << std::endl;
    std::cout << req->content_length_ << std::endl;
    std::cout << req->body_ << std::endl;
  }
  return 0;
}
