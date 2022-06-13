#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>
# include <vector>
# include <map>

typedef enum e_method {
	METHOD_INVALID,		//未定義
	METHOD_GET,
	METHOD_POST,
	METHOD_DELETE,
} method_type;

typedef enum e_uri {
	URI_FILE,	//todo: text,imageの区別は？
	URI_IMAGE,
	URI_DIR,
	URI_REDIRECT,
	URI_CGI,
} uri_type;


class HttpRequest
{
public:
	std::string method_;
	method_type	method_type_;
	std::string uri_;
	uri_type uri_type_;
	std::string version_;
	std::string content_type_;
	std::vector<std::string> header_;
	std::vector<std::string> body_;
};

#endif
