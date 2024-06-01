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
#include "Webserv.hpp"

#define DEFAULT_CONFIG_FILE "config/default.conf"

int main(int argc, char **argv)
{
	// Check if there is a parameter and if it is a .conf file
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <config_file.conf>" << std::endl; //NOTE msg
		return 1;
	}
	if (getExtension(argv[1]) != "conf")
	{
        std::cout << "Config file does not have .conf extension: " << argv[1] << std::endl; //NOTE msg
        return 1;
    }

	// Initialize webserv with the config file
	try
	{
		Webserv webserv;

		std::cout << "Initializing server" << std::endl; //NOTE msg
		webserv.init(argv[1]);
		std::cout << webserv << std::endl; //XXX for test only
		std::cout << "Starting server loop" << std::endl; //NOTE msg
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
