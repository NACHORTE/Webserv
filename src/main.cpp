#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Server.hpp"
#include <sstream>
#include <cstring>
#include <fstream>
#include <vector>
#include <poll.h>
#include "defines.h"
#include "colors.h"
#include "utils.hpp"
#include "set_config.hpp"
#include "Webserv.hpp"

/* static void print_long_str(const std::string & str, size_t max_size = 1000)
{
	if (str.size() > max_size)
		std::cout << str.substr(str.size()- max_size) << "...";
	else
		std::cout << str;
} */

#define DEFAULT_CONFIG_FILE "config/default.conf"

int main(int argc, char **argv)
{
	// Check if there is a parameter and if it is a .conf file
	std::cout << "checking argc == 2" << std::endl;
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <config_file.conf>" << std::endl;
		return 1;
	}
	std::cout << "checking file extension" << std::endl;
	if (getExtension(argv[1]) != "conf")
	{
        std::cout << "Config file does not have .conf extension: " << argv[1] << std::endl;
        return 1;
    }

	// Initialize webserv with the config file
	try
	{
		Webserv webserv;

		std::cout << "Initializing server" << std::endl;
		webserv.init("config/default.conf");
		std::cout << webserv << std::endl;
		std::cout << "Server loop" << std::endl;
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
