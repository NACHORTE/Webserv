#include "readConfig.hpp"

void readAlias(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Location & loc)
{
	if (it == end)
		throw std::runtime_error("Error reading config file, expected alias after alias token");
	loc.setAlias(*(it++));
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, expected ; after alias");
	++it;
}
