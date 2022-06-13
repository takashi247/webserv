#include "httpRequestParser.hpp"

req_type HttpRequestParser::getMethod(std::string& recv_msg)
{
	(void)recv_msg;
	return REQ_GET;
}
std::string HttpRequestParser::getUri(std::string& recv_msg)
{
	(void)recv_msg;
	return "./index.html";
}
HttpRequest *HttpRequestParser::getHttpRequest(std::string& recv_msg)
{
	HttpRequest *req = new HttpRequest();
	req->type_ = getMethod(recv_msg);
	req->uri_ = getUri(recv_msg);
	return req;
}
