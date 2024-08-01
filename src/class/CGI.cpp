#include "CGI.hpp"

CGI::CGI(void)
{
}

CGI::CGI(const CGI & src)
{
	*this = src;
}

CGI::~CGI()
{
}

CGI &CGI::operator=(const CGI &rhs)
{
	if (this != &rhs)
	{
		// copy
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const CGI &obj)
{
	(void)obj;
	return (os);
}
