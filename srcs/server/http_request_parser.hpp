#ifndef HTTP_REQUEST_PARSER_HPP
# define HTTP_REQUEST_PARSER_HPP

# include "http_request.hpp"

class HttpRequestParser
{
private:
	static std::string GetMethod(std::string& recv_msg);
	static std::string GetUri(std::string& recv_msg);
	static std::string GetProtocolVersion(std::string& recv_msg);
	static std::string GetFieldValue(const char* field_name, std::string& recv_msg);
public:
	static HttpRequest* CreateHttpRequest(std::string& recv_msg);
	static void DestroyHttpRequest(HttpRequest *req);
};

#endif
