#include "readConfig.hpp"
Server readServer(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end)
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
		duplicableAttributes.insert("error_page");

		mandatoryAttributes.insert("listen");
	}

	Server server;
	// Attributes that have been used in the server block
	std::set<std::string> usedAttributes;

	// Skip the { token
	if (it == end || *(it++) != "{")
		throw std::runtime_error("Error reading config file, expected \"{\" after server");

	// Start parsing the content
	while (it != end and *it != "}")
	{
		if (usedAttributes.count(*it) == 1 and duplicableAttributes.count(*it) == 0)
			throw std::runtime_error("Error reading config file, duplicated token in server block: " + *it);
		if (attributeFunctions.count(*it) == 1)
		{
			std::string attribute = *it;
			attributeFunctions[attribute](++it, end, server);
			usedAttributes.insert(attribute);
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
