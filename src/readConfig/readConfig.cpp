#include "readConfig.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

static std::list<std::string> tokenize(const std::string &configFile)
{
	std::ifstream file(configFile.c_str());
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
			servers.push_back(readServer(++it, tokens.end()));
		// Only server tokens are allowed at the top level
		else
			throw std::runtime_error("Error reading config file, unexpected token: " + *it);
	}

	return servers;
}
