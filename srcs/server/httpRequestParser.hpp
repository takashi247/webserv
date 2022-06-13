#ifndef HTTPREQUESTPARSER_HPP
# define HTTPREQUESTPARSER_HPP

# include "httpRequest.hpp"

class HttpRequestParser
{
private:
	req_type getMethod(std::string& recv_msg);
	std::string getUri(std::string& recv_msg);

public:
	static HttpRequest *getHttpRequest(std::string& recv_msg);
};

#endif
