#include "http_request_parser.hpp"

#include <iostream>

std::string HttpRequestParser::GetMethod(const std::string& recv_msg) {
  int pos = recv_msg.find(' ');
  std::string method(recv_msg.begin(), recv_msg.begin() + pos);
  return method;
}
std::string HttpRequestParser::GetUri(const std::string& recv_msg) {
  int start_pos = recv_msg.find(' ');
  int end_pos = recv_msg.find(' ', start_pos + 1);
  std::string uri(recv_msg.begin() + start_pos + 1, recv_msg.begin() + end_pos);
  return uri;
}
std::string HttpRequestParser::GetProtocolVersion(const std::string& recv_msg) {
  int start_pos = recv_msg.find(' ');
  start_pos = recv_msg.find(' ', start_pos + 1);
  int end_pos = recv_msg.find('\r', start_pos + 1);
  std::string version(recv_msg.begin() + start_pos + 1,
                      recv_msg.begin() + end_pos);
  return version;
}
bool HttpRequestParser::IsValidHttpVersion(
    const std::string& protocol_version) {
  int pos = protocol_version.find("HTTP/");
  char version_head_num = protocol_version[kProtocolVersionPos];
  if (pos == 0 && ('0' < version_head_num && version_head_num <= '9'))
    return true;
  return false;
}
std::string HttpRequestParser::GetFieldValue(const char* field_name,
                                             const std::string& recv_msg) {
  int name_start_pos = recv_msg.find(field_name, 0, strlen(field_name));
  if (-1 == name_start_pos) return "";
  int value_start_pos = recv_msg.find(": ", name_start_pos + 1) + 2;
  int value_end_pos = recv_msg.find('\r', value_start_pos + 1);
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
void HttpRequestParser::GetMessageBody(const std::string& recv_msg,
                                       std::string& body) {
  int pos = recv_msg.find("\r\n\r\n");
  body.assign(recv_msg.begin() + pos + 4, recv_msg.end());
}

int HttpRequestParser::Parse(const std::string& recv_msg, HttpRequest* req) {
  req->method_ = GetMethod(recv_msg);
  req->uri_ = GetUri(recv_msg);
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
  std::string host = GetFieldValue("Host", recv_msg);
  {  // separate host -> name:port
    size_t pos = host.find(":");
    if (pos == std::string::npos)
      req->host_name_ = host;
    else {
      req->host_name_ = host.substr(0, pos++);
      req->host_port_ = atoi(host.substr(pos, host.size()).c_str());
    }
  }
  req->content_type_ = GetFieldValue("Content-Type", recv_msg);
  req->content_length_ = GetFieldValueSize("Content-Length", recv_msg);
  GetMessageBody(recv_msg, req->body_);

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
