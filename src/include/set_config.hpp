#pragma once
#include <string>
#include "utils.hpp"
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include "defines.h"
#include <arpa/inet.h>

typedef struct s_location
{
	std::string path;
	std::string root;
	std::string index;
	std::string autoindex;
	std::string methods;
}	t_location;

typedef struct s_server
{
	int sockfd;
	int port;
    std::string sv_name;
    std::string host;
    std::string root;
    std::string error_page;
    std::string index;
	std::vector<t_location> locations;
    int max_body;
}	t_server;

std::vector<t_server> read_config(const std::string& config_file);