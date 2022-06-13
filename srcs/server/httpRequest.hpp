#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>
# include <vector>

typedef enum e_type{
	REQ_GET,
	REQ_POST,
	REQ_DELETE,
}req_type;

class HttpRequest {
public:
	req_type	type_;
	std::string method_;
	std::string uri_;
	std::string version_;
	std::string content_type_;
	std::vector<std::string> header_;
	std::vector<std::string> body_;
};

#endif
