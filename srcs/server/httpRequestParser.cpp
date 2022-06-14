#include "httpRequestParser.hpp"

#include <iostream>

std::string HttpRequestParser::GetMethod(std::string& recv_msg)
{
	int pos = recv_msg.find_first_of(' ');
	std::string method(recv_msg.begin(), recv_msg.begin() + pos);
	return method;
}
std::string HttpRequestParser::GetUri(std::string& recv_msg)
{
	int start_pos = recv_msg.find_first_of(' ');
	int end_pos = recv_msg.find_first_of(' ', start_pos + 1);
	std::string uri(recv_msg.begin() + start_pos + 1, recv_msg.begin() + end_pos);
	return uri;
}
std::string HttpRequestParser::GetProtocolVersion(std::string& recv_msg)
{
	int start_pos = recv_msg.find_first_of(' ');
	start_pos = recv_msg.find_first_of(' ', start_pos + 1);
	int end_pos = recv_msg.find_first_of('\n', start_pos + 1);
	std::string version(recv_msg.begin() + start_pos + 1, recv_msg.begin() + end_pos);
	return version;
}
std::string HttpRequestParser::GetFieldValue(const char* field_name, std::string& recv_msg)
{
	int name_start_pos = recv_msg.find(field_name, 0, strlen(field_name));
	if (-1 == name_start_pos)
		return "";
	int value_start_pos = recv_msg.find(": ", name_start_pos + 1) + 2;
	int value_end_pos = recv_msg.find_first_of('\n', value_start_pos + 1);
	std::string value(recv_msg.begin() + value_start_pos, recv_msg.begin() + value_end_pos);
	return value;
}

HttpRequest *HttpRequestParser::CreateHttpRequest(std::string& recv_msg)
{
	HttpRequest *req = new HttpRequest();

	req->method_ = GetMethod(recv_msg);
	req->uri_ = GetUri(recv_msg);
	req->version_ = GetProtocolVersion(recv_msg);
	//TODO; check Protocol Version
	req->content_type_ = GetFieldValue("Content-Type", recv_msg);

	std::cout << req->method_ << std::endl;
	std::cout << req->uri_ << std::endl;
	std::cout << req->version_ << std::endl;
	std::cout << req->content_type_ << std::endl;

	return req;
}
void HttpRequestParser::DestroyHttpRequest(HttpRequest *req)
{
	delete(req);
}