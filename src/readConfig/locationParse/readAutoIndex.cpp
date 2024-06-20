#include "readConfig.hpp"

void readAutoIndex(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Location & loc)
{
	if (it == end)
		throw std::runtime_error("Error reading config file, expected value after autoindex token");
	if (*it == "on")
		loc.autoIndex(true);
	else if (*it == "off")
		loc.autoIndex(false);
	else
		throw std::runtime_error("Error reading config file, expected on or off after autoindex token");
	++it;
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, expected ; after autoindex");
	++it;
}