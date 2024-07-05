#include "readConfig.hpp"

int toInt(const std::string & str)
{
	int count = 0;
	for (size_t i = 0; i < str.size();++i)
	{
		if (str[i] < '0' or str[i] > '9')
			throw std::runtime_error("Error reading config file, expected number after error_page");
		count = count * 10 + str[i] - '0';
		if (count > 599)
			throw std::runtime_error("Error reading config file, invalid number after error_page");
	}
	return count;
}

void readErrorPage(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv)
{
	if (it == end)
		throw std::runtime_error("Error reading config file, expected number after error_page");

	std::list<std::string>::iterator next = it;
	++next;
	std::set<int> errorCodes;	
	size_t count = 0;
	while (next != end and *next != ";" and *next != "{" and *next != "}")
	{
		errorCodes.insert(toInt(*it));
		++it;
		++next;
		++count;
	}
	if (count == 0)
		throw std::runtime_error("Error reading config file, expected number after error_page");
	for (std::set<int>::iterator code = errorCodes.begin(); code != errorCodes.end(); ++code)
		serv.addErrorPage(*code, *it);
	++it;
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, expected ; after error_page");
	++it;
}
