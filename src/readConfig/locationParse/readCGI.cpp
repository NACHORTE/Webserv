#include "readConfig.hpp"

void readCGI(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Location & loc)
{
	if (it == end)
		throw std::runtime_error("Error reading config file, expected value after cgi token");
	if (*it == "on")
		loc.isCgi(true);
	else if (*it == "off")
		loc.isCgi(false);
	else
		throw std::runtime_error("Error reading config file, expected on or off after cgi token");
	++it;
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, expected ; after cgi token");
	++it;
}