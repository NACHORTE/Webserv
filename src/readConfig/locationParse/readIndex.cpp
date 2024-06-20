#include "readConfig.hpp"

void readIndex(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Location & loc)
{
	if (it == end)
		throw std::runtime_error("Error reading config file, expected index after index token");
	loc.setIndex(*(it++));
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, expected ; after index");
	++it;
}