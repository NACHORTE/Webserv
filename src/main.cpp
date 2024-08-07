#include <iostream>
#include "utils.hpp"
#include "Webserv.hpp"

int main(int argc, char **argv)
{
	// Check if there is a parameter and if it is a .conf file
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <config_file.conf>" << std::endl;
		return 1;
	}
	if (getExtension(argv[1]) != "conf")
	{
        std::cout << "ERROR: Config file does not have .conf extension: " << argv[1] << std::endl;
        return 1;
    }

	// Initialize webserv with the config file
	try
	{
		Webserv webserv;

		DEBUG("initializing webserv with config file: " << argv[1]);
		webserv.init(argv[1]);
		std::cout << webserv << std::endl;
		DEBUG("webserv initialized");
		while(1)
			webserv.loop();
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}

	return 0;
}
