#include <iostream>
#include <fstream>
#include <sstream>
#include "Server.hpp"
#include "utils.hpp"

static void readListen(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv)
{
	if (*(it++) != "listen")
		throw std::runtime_error("Error reading config file, expected: listen");
	if (it == end)
		throw std::runtime_error("Error reading config file, missing port number after listen directive");
	size_t port = 0;
	for (size_t i = 0; i < it->size(); ++i)
	{
		if ((*it)[i] < '0' or (*it)[i] > '9')
			throw std::runtime_error("Error reading config file, invalid port number: " + *it);
		port = port * 10 + (*it)[i] - '0';
		if (port > 65535)
			throw std::runtime_error("Error reading config file, invalid port number: " + *it);
	}
	if (it == end || *it != ";")
		throw std::runtime_error("Error reading config file, missing \";\" at the end of the listen directive");
	serv.setPort(port);
	++it;
}

static void readLocation(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv)
{
	return ;
	/* std::string word;

	if (iss >> word && word[0] == '/')
	{
		location->setURI(word);
	}
	else
	{
		std::cout << "Error reading config file, missing/error: path" << std::endl;
		return 0;
	}
	if (iss >> word && word != "{")
	{
		std::cout << "Error reading config file (location), missing/error: {" << std::endl;
		return 0;
	}
	while (iss >> word)
	{
		if (word == "}")
		{
			return 1;
			break;
		}
		else if (word == "root")
		{
			if (check_duplicated(location->_root))
			{
				std::cout << "Error reading config file (location), duplicated: root" << std::endl;
				return 0;
			}
			if (iss >> word && back(word) == ';')
			{
				location->_root = word;
				//std::cout << "root: " << location->root << std::endl;
			}
			else
			{
				std::cout << "Error reading config file (location), missing/error: root" << std::endl;
				return 0;
			}
		}
		else if (word == "index")
		{
			if (check_duplicated(location->_index))
			{
				std::cout << "Error reading config file (location), duplicated: index" << std::endl;
				return 0;
			}
			if (iss >> word && back(word) == ';')
			{
				location->_index = word;
			}
			else
			{
				std::cout << "Error reading config file (location), missing/error: index" << std::endl;
				return 0;
			}
		}
 		else if (word == "autoindex")
		{
			if (check_duplicated(location->_autoIndex))
			{
				std::cout << "Error reading config file (location), duplicated: autoindex" << std::endl;
				return 0;
			}
			if (iss >> word && back(word) == ';')
			{
				location->_autoIndex = (word == "on" ? 1 : 0);
				//std::cout << "autoindex: " << location->autoindex << std::endl;
			}
			else
			{
				std::cout << "Error reading config file (location), missing/error: autoindex" << std::endl;
				return 0;
			}
		}
		else if (word == "methods")
		{
 			if (check_duplicated(*location->_allowedMethods.begin()))
			{
				std::cout << "Error reading config file (location), duplicated: methods" << std::endl;
				return 0;
			} 
			if (iss >> word && back(word) == ';')
			{
				location->_allowedMethods.insert(word);
				//std::cout << "methods: " << location->methods << std::endl;
			}
			else
			{
				std::cout << "Error reading config file (location), missing/error: methods" << std::endl;
				return 0;
			}
		}
		else
		{
			std::cout << "Error reading config file (location), unknown: " << word << std::endl;
			return 0;
		}
	}
	return 0;*/
}

static Server readServer(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end)
{
	static bool initialized = false;
	// Function pointers to read the different attributes of a server
	static std::map<std::string,
		void (*)(std::list<std::string>::iterator &,
			const std::list<std::string>::iterator &,Server &)> attributeFunctions;
	// Atributes that can appear multiple times
	static std::set<std::string> duplicableAttributes;
	// Atributes that have to appear at least once
	static std::set<std::string> mandatoryAttributes;

	if (not initialized)
	{
		initialized = true;

		attributeFunctions["listen"] = readListen;
		attributeFunctions["server_name"] = readServerName;
		attributeFunctions["root"] = readRoot;
		attributeFunctions["host"] = readHost;
		attributeFunctions["error_page"] = readErrorPage;
		attributeFunctions["index"] = readIndex;
		attributeFunctions["max_body"] = readMaxBody;
		attributeFunctions["location"] = readLocation;

		duplicableAttributes.insert("location");

		mandatoryAttributes.insert("listen");
	}

	Server server;
	// Attributes that have been used in the server block
	std::set<std::string> usedAttributes;

	// Skip the server and { tokens
	if (*(it++) != "server")
		throw std::runtime_error("Error reading config file, expected: server");
	if (*(it++) != "{")
		throw std::runtime_error("Error reading config file, expected \"{\" after server");

	// Start parsing the content
	while (it != end and *it != "}")
	{
		if (usedAttributes.count(*it) == 1 and duplicableAttributes.count(*it) == 0)
			throw std::runtime_error("Error reading config file, duplicated token in server block: " + *it);
		if (attributeFunctions.count(*it) == 1)
		{
			attributeFunctions[*it](it, end, server);
			usedAttributes.insert(*it);
		}
		else
			throw std::runtime_error("Error reading config file, unknown token in server block: " + *it);
	}

	// Check if there are any mandatory attributes missing
	for (std::set<std::string>::iterator it = mandatoryAttributes.begin(); it != mandatoryAttributes.end(); it++)
		if (usedAttributes.count(*it) == 0)
			throw std::runtime_error("Error reading config file, missing mandatory token in server block: " + *it);
	
	if (it == end || *it != "}")
		throw std::runtime_error("Error reading config file, missing \"}\" at the end of the server block");
	// Skip the } token
	it++;
	return server;
}

static std::list<std::string> tokenize(const std::string &configFile)
{
	std::ifstream file(configFile);
	if (!file.is_open())
		throw std::ios_base::failure("Error: could not open file " + configFile);

	std::string word;
	std::list<std::string> tokens;
	std::string special_chars = "{};";
	while (file >> word)
	{
		size_t begin = 0;
		size_t end = word.find_first_of(special_chars);
		while (end!= std::string::npos)
		{
			if (begin != end)
				tokens.push_back(word.substr(begin,end - begin));
			tokens.push_back(word.substr(end,1));
			begin = end + 1;
			end = word.find_first_of(special_chars, begin);
		}
		if (begin == 0)
			tokens.push_back(word);
		else if (begin < word.size())
			tokens.push_back(word.substr(begin));
	}

	return tokens;
}

std::vector<Server> readConfig(const std::string& configFile)
{
	std::vector<Server> servers;
	// Parse the file into tokens (throws exception if file cannot be opened)
	std::list<std::string> tokens = tokenize(configFile);
	// Flag that goes true for the first server without a name
	bool nameLessServer = false;
	
	// Keep adding servers until we run out of tokens
	std::list<std::string>::iterator it = tokens.begin();
	while(it != tokens.end())
	{
		if (*it == "server")
			servers.push_back(readServer(it, tokens.end()));
		// Only server tokens are allowed at the top level
		else
			throw std::runtime_error("Error reading config file, unknown: " + *it);
		if (servers.back().getServerNames().empty())
		{
			if (nameLessServer == true)
				throw std::runtime_error("Error reading config file, multiple servers without a name");
			nameLessServer = true;
		}
	}

	return servers;
}
