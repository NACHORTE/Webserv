#include "readConfig.hpp"

void readReturn(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Location & loc)
{
	if (it == end)
		throw std::runtime_error("Error reading config file, expected number after return token");
	
	size_t count=0;
	for (size_t i = 0; i < it->size();++i)
	{
		if ((*it)[i] < '0' or (*it)[i] > '9')
			throw std::runtime_error("Error reading config file, expected number after return token");
		count = count * 10 + (*it)[i] - '0';
		if (count > 599)
			throw std::runtime_error("Error reading config file, invalid number after return token");
	}
	++it;
	if (it == end or *it == ";")
		throw std::runtime_error("Error reading config file, expected value after return token");
	loc.setReturnValue(count, *it);
	++it;
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, expected ; after return");
	++it;
}