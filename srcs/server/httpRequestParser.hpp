#ifndef HTTPREQUESTPARSER_HPP
# define HTTPREQUESTPARSER_HPP

# include "httpRequest.hpp"

class HttpRequestParser
{
private:
	static method_type convertMethod(std::string& method);
	static method_type getMethod(std::string& recv_msg);
	static std::string getUri(std::string& recv_msg);
	static uri_type convertUriType(std::string& uri);
	static std::string getProtocolVersion(std::string& recv_msg);

	static std::string getFieldValue(const char* field_name, std::string& recv_msg);
public:
	static HttpRequest* createHttpRequest(std::string& recv_msg);
	static void destroyHttpRequest(HttpRequest *req);
};

#endif
