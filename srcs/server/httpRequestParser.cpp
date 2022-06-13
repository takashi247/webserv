#include "httpRequestParser.hpp"

#include <iostream>

method_type HttpRequestParser::convertMethod(std::string& method)
{
	if (method == "GET")
		return METHOD_GET;
	else if (method == "POST")
		return METHOD_POST;
	else if (method == "DELETE")
		return METHOD_DELETE;
	else
		return METHOD_INVALID;
}
method_type HttpRequestParser::getMethod(std::string& recv_msg)
{
	int pos = recv_msg.find_first_of(' ');
	std::string method(recv_msg.begin(), recv_msg.begin() + pos);
	return convertMethod(method);
}
std::string HttpRequestParser::getUri(std::string& recv_msg)
{
	int start_pos = recv_msg.find_first_of(' ');
	int end_pos = recv_msg.find_first_of(' ', start_pos + 1);
	std::string uri(recv_msg.begin() + start_pos + 1, recv_msg.begin() + end_pos);
	return uri;
}
uri_type HttpRequestParser::convertUriType(std::string& uri)
{
	//TODO: configと合わせて参照しないと、リダイレクトとか判定できないかも。
	(void)uri;
	return URI_FILE;
}
std::string HttpRequestParser::getProtocolVersion(std::string& recv_msg)
{
	int start_pos = recv_msg.find_first_of(' ');
	start_pos = recv_msg.find_first_of(' ', start_pos + 1);
	int end_pos = recv_msg.find_first_of('\n', start_pos + 1);
	std::string version(recv_msg.begin() + start_pos + 1, recv_msg.begin() + end_pos);
	return version;
}
std::string HttpRequestParser::getFieldValue(const char* field_name, std::string& recv_msg)
{
	int name_start_pos = recv_msg.find(field_name, 0, strlen(field_name));
	if (-1 == name_start_pos)
		return "";
	int value_start_pos = recv_msg.find_first_of(": ", name_start_pos + 1) + 2;
	int value_end_pos = recv_msg.find_first_of('\n', value_start_pos + 1);
	std::string value(recv_msg.begin() + value_start_pos, recv_msg.begin() + value_end_pos);
	return value;
}

HttpRequest *HttpRequestParser::createHttpRequest(std::string& recv_msg)
{
	HttpRequest *req = new HttpRequest();

	req->method_type_ = getMethod(recv_msg);
	req->uri_ = getUri(recv_msg);
	req->uri_type_ = convertUriType(req->uri_);
	req->version_ = getProtocolVersion(recv_msg);
	//TODO; check Protocol Version
	req->content_type_ = getFieldValue("Content-Type", recv_msg);

	return req;
}
void HttpRequestParser::destroyHttpRequest(HttpRequest *req)
{
	delete(req);
}