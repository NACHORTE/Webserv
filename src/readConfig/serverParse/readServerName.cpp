#include "readConfig.hpp"
#include "utils.hpp"

static bool checkName(const std::string & name)
{
	if (name.size() > 253)
		return false;

	std::list<std::string> segments;
	size_t begin = 0;
	size_t end = name.find_first_of('.');
	while (end != std::string::npos)
	{
		std::string segment = name.substr(begin, end - begin);
		segments.push_back(segment);
		if (segment.empty() or segment[0] == '-' or back(segment) == '-'
			or segments.size() > 127 or segment.size() > 63)
			return false;
		for (size_t i = 0; i < segment.size(); ++i)
			if ((segment[i] < 'a' or segment[i] > 'z') and (segment[i] < 'A' or segment[i] > 'Z')
				and (segment[i] < '0' or segment[i] > '9') and segment[i] != '-')
				return false;
		begin = end + 1;
		end = name.find_first_of('.', begin);
	}
	//check if it is an ip address
	if (segments.size() == 4)
	{
		bool isIp = true;
		bool error = false;
		for (std::list<std::string>::iterator it = segments.begin(); it != segments.end(); ++it)
		{
			int num = 0;
			for (size_t i = 0; i < it->size(); ++i)
			{
				if ((*it)[i] < '0' or (*it)[i] > '9')
				{
					isIp = false;
					break;
				}
				num = num * 10 + (*it)[i] - '0';
				if (num > 255)
				{
					error = true;
					break;
				}
			}
			if (not isIp)
				break;
		}
		if (isIp and error)
			return false;
	}
	return true;
}

void readServerName(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv)
{
	size_t count = 0;
	while (it != end and *it != ";")
	{
		if (not checkName(*it))
			throw std::runtime_error("Error reading config file, invalid server name: " + *it);
		serv.addServerName(*it);
		++count;
		++it;
	}
	if (count == 0)
		throw std::runtime_error("Error reading config file, missing server name after server_name directive");
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, missing \";\" at the end of the server_name directive");
	++it;
}