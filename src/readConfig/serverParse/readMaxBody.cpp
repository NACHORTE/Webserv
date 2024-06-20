#include "readConfig.hpp"
#include "utils.hpp"

void readMaxBody(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv)
{
	if (it == end or *it == ";")
		throw std::runtime_error("Error reading config file, missing value after client_read_max_body directive");
	// if the last character is not a multiplier or a number, throw an error
	if ((back(*it) < '0' or back(*it) > '9') and std::string("kmg").find(back(*it)) == std::string::npos)
		throw std::runtime_error("Error reading config file, invalid value for client_read_max_body: " + *it);
	// check if the rest of the string is a number
	for (size_t i = 0; i + 1< it->size(); ++i)
		if ((*it)[i] < '0' or (*it)[i] > '9')
			throw std::runtime_error("Error reading config file, invalid value for client_read_max_body: " + *it);
	
	// Set the multiplier based on the last character
	int multiplier = 1;
	if (back(*it) == 'k')
		multiplier = 1024;
	else if (back(*it) == 'm')
		multiplier = 1024 * 1024;
	else if (back(*it) == 'g')
		multiplier = 1024 * 1024 * 1024;
	
	// Get the value
	size_t val = 0;
	for (size_t i = 0 ; i < it->size() and (*it)[i] >= '0' and (*it)[i] <= '9'; ++i)
		val = val * 10 + (*it)[i] - '0';
	val *= multiplier;
	serv.setMaxBodySize(val);

	// Check if the attribute ends with ;
	++it;
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, missing \";\" at the end of the client_read_max_body directive");
	++it;
}
