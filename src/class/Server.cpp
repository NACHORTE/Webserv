#include "Server.hpp"

Server::Server(void)
{
}

Server::Server(const Server & src)
{
	*this = src;
}

Server::~Server()
{
}

Server &Server::operator=(const Server &rhs)
{
	if (this != &rhs)
	{
		// copy
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Server &obj)
{
	(void)obj;
	return (os);
}
