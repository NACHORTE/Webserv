#pragma once
#include <iostream>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Locations.hpp"
#include <map>
#include "set_config.hpp"

class Server
{
	public:
		Server();
		Server(const Server & src);
		~Server();
		Server & operator=(const Server & rhs);
	protected:
	private:
	public:
		std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)> _allowed_methods;
		Locations _allowed_paths;
		std::vector<std::string> _errorPages;
		int sockfd;
		int port;
    	std::string sv_name;
    	std::string host;
    	std::string root;
    	std::string error_page;
    	std::string index;
		std::vector<t_location> locations;
    	int max_body;

	friend std::ostream &operator<<(std::ostream &os, const Server &obj);
};

std::ostream &operator<<(std::ostream &os, const Server &obj);

std::vector<Server> read_config(const std::string& config_file);