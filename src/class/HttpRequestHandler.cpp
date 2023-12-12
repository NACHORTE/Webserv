#include "HttpRequestHandler.hpp"

HttpRequestHandler::HttpRequestHandler(void)
{
}

HttpRequestHandler::HttpRequestHandler(const HttpRequestHandler & src)
{
	*this = src;
}

HttpRequestHandler::~HttpRequestHandler(void)
{
}

HttpRequestHandler &HttpRequestHandler::operator=(const HttpRequestHandler &rhs)
{
	if (this != &rhs)
	{
		// copy
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const HttpRequestHandler &obj)
{
	return (os);
}
