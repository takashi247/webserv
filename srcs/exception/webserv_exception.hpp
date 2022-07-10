#ifndef WEBSERV_EXCEPTION_HPP_
#define WEBSERV_EXCEPTION_HPP_

#include <iostream>
#include <exception>

class WebservException
{
private:
	std::string msg_;

public:
	WebservException(const  std::string &msg = NULL);
	~WebservException();
	WebservException(WebservException const &rhs);
	WebservException &operator=(WebservException const &rhs);

	virtual const char* what(void) const throw();
};

#endif
