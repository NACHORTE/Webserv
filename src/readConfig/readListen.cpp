#include "readConfig.hpp"

void readListen(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv)
{
	if (*(it++) != "listen")
		throw std::runtime_error("Error reading config file, expected: listen");
	if (it == end)
		throw std::runtime_error("Error reading config file, missing port number after listen directive");
	size_t port = 0;
	for (size_t i = 0; i < it->size(); ++i)
	{
		if ((*it)[i] < '0' or (*it)[i] > '9')
			throw std::runtime_error("Error reading config file, invalid port number: " + *it);
		port = port * 10 + (*it)[i] - '0';
		if (port > 65535)
			throw std::runtime_error("Error reading config file, invalid port number: " + *it);
	}
	if (it == end || *it != ";")
		throw std::runtime_error("Error reading config file, missing \";\" at the end of the listen directive");
	serv.setPort(port);
	++it;
}