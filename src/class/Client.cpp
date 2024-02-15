#include "Client.hpp"

Client::Client(void)
{
}

Client::Client(const Client & src)
{
	*this = src;
}

Client::~Client()
{
}

Client &Client::operator=(const Client &rhs)
{
	if (this != &rhs)
	{
		// copy
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Client &obj)
{
	(void)obj;
	return (os);
}
