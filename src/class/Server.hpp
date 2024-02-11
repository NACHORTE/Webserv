#pragma once
#include <iostream>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Locations.hpp"
#include <map>

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
		std::string _host;
		int _port;
		std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)> _allowed_methods;
		Locations _allowed_paths;
		std::vector<std::string> _errorPages;  

	friend std::ostream &operator<<(std::ostream &os, const Server &obj);
};

std::ostream &operator<<(std::ostream &os, const Server &obj);
