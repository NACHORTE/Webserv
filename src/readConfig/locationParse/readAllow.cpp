#include "readConfig.hpp"
#include "utils.hpp"

void readAllow(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Location & loc)
{
	std::set<std::string> allowedMethods = getAllowedHttpMethods();
	if (it == end)
		throw std::runtime_error("Error reading config file, expected value after allow token");
	while (it != end and *it != ";" and *it != "{" and *it != "}")
	{
		if (allowedMethods.count(*it) == 0)
			throw std::runtime_error("Error reading config file, invalid method in allow: " + *it);
		loc.addAllowedMethod(*it);
		++it;
	}
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, expected ; after allow");
	++it;
}
