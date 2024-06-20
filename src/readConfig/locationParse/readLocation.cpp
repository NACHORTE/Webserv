#include "readConfig.hpp"

void readLocation(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & server)
{
	static bool initialized = false;
	// Function pointers to read the different attributes of a server
	static std::map<std::string,
		void (*)(std::list<std::string>::iterator &,
			const std::list<std::string>::iterator &,Location &)> attributeFunctions;
	// Atributes that can appear multiple times
	static std::set<std::string> duplicableAttributes;
	// Atributes that have to appear at least once
	static std::set<std::string> mandatoryAttributes;

	if (not initialized)
	{
		initialized = true;

		attributeFunctions["alias"] = readAlias;
		attributeFunctions["root"] = readRoot;
		attributeFunctions["index"] = readIndex;
		attributeFunctions["autoindex"] = readAutoIndex;
		attributeFunctions["cgi"] = readCGI;
		attributeFunctions["allow"] = readAllow;
		attributeFunctions["return"] = readReturn;

		duplicableAttributes.insert("allow");
	}

	Location loc;
	// Inherit the root and index from the server
	loc.setRoot(server.getRoot());
	loc.setIndex(server.getIndex());

	// Attributes that have been used in the server block
	std::set<std::string> usedAttributes;

	if (it == end)
		throw std::runtime_error("Error reading config file, expected URI after location token");
	loc.setURI(*(it++));

	// Skip the { token
	if (it == end or *(it++) != "{")
		throw std::runtime_error("Error reading config file, expected \"{\" after location URI");

	// Start parsing the content
	while (it != end and *it != "}")
	{
		if (usedAttributes.count(*it) == 1 and duplicableAttributes.count(*it) == 0)
			throw std::runtime_error("Error reading config file, duplicated token in location block: " + *it);
		if (attributeFunctions.count(*it) == 1)
		{
			std::string attribute = *it;
			attributeFunctions[attribute](++it, end, loc);
			usedAttributes.insert(attribute);
		}
		else
			throw std::runtime_error("Error reading config file, unknown token in location block: " + *it);
	}

	// Check if there are any mandatory attributes missing
	for (std::set<std::string>::iterator it = mandatoryAttributes.begin(); it != mandatoryAttributes.end(); it++)
		if (usedAttributes.count(*it) == 0)
			throw std::runtime_error("Error reading config file, missing mandatory token in location block: " + *it);
	
	if (it == end || *it != "}")
		throw std::runtime_error("Error reading config file, missing \"}\" at the end of location block");
	// Skip the } token
	it++;
	server.addLocation(loc);
}