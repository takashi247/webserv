#include "webserv_exception.hpp"

WebservException::WebservException(const std::string &msg) : msg_(msg) {}

WebservException::~WebservException(){}

WebservException::WebservException(WebservException const &rhs)
{
	*this = rhs;
}

WebservException &WebservException::operator=(WebservException const &rhs)
{
	if (this != &rhs)
	{
		msg_ = rhs.msg_;
	}
	return *this;
}

const char *WebservException::what(void) const throw() {
	return (msg_.c_str());
}
