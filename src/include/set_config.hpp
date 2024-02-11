#pragma once
#include <string>
#include "utils.hpp"
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include "defines.h"
#include <arpa/inet.h>

class t_location
{
	public:
	std::string path;
	std::string root;
	std::string index;
	std::string autoindex;
	std::string methods;
};
