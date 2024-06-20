#include "readConfig.hpp"
#include <iostream>

void readIndex(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv)
{
	if (it == end or *it == ";")
		throw std::runtime_error("Error reading config file, missing index path after index directive");
	std::cerr << "hola "<<*it << std::endl;
	serv.setIndex(*(it++));
	std::cerr << *it << std::endl;
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, missing \";\" at the end of the index directive");
	std::cerr << *it << std::endl;
	++it;
}
