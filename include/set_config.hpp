#pragma once
#include <string>
#include "utils.hpp"
#include <vector>

typedef struct s_server
{
	int port;
    std::string sv_name;
    std::string host;
    std::string root;
    std::string error_page;
    std::string index;
    int max_body;
}	t_server;

std::vector<t_server> read_config(const std::string& config_file);