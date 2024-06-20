#include "readConfig.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

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
		attributeFunctions["index"] = readIndex;
		attributeFunctions["error_page"] = readErrorPage;
		attributeFunctions["client_max_body_size"] = readMaxBody;
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
	std::string special_chars = "{};#";
	while (file >> word)
	{
		size_t begin = 0;
		size_t end = word.find_first_of(special_chars);
		while (end != std::string::npos and word[end] != '#')
		{
			if (begin != end)
				tokens.push_back(word.substr(begin,end - begin));
			tokens.push_back(word.substr(end,1));
			begin = end + 1;
			end = word.find_first_of(special_chars, begin);
		}
		if (word[end] == '#')
		{
			if (begin != end)
				tokens.push_back(word.substr(begin,end - begin));
			std::getline(file, word);
			continue;
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
	
	// Keep adding servers until we run out of tokens
	std::list<std::string>::iterator it = tokens.begin();
	while(it != tokens.end())
	{
		if (*it == "server")
			servers.push_back(readServer(it, tokens.end()));
		// Only server tokens are allowed at the top level
		else
			throw std::runtime_error("Error reading config file, unknown: " + *it);
	}

	return servers;
}
