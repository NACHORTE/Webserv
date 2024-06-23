#include <readConfig.hpp>

void readRoot(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv)
{
	if (it == end or *it == ";")
		throw std::runtime_error("Error reading config file, missing root path after root directive");
	serv.setRoot(*(it++));
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, missing \";\" at the end of the root directive");
	++it;
}