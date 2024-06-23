#include "readConfig.hpp"

void readAllow(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Location & loc)
{
	if (it == end)
		throw std::runtime_error("Error reading config file, expected value after allow token");
	while (it != end and *it != ";")
	{
		if (*it != "GET" and *it != "POST" and *it != "DELETE")
			throw std::runtime_error("Error reading config file, expected GET, POST or DELETE after allow");
		loc.addAllowedMethod(*it);
		++it;
	}
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, expected ; after allow");
	++it;
}
