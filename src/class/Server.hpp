#pragma once
#include <iostream>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Location.hpp"
#include <map>

class Server
{
	public:
		Server(void);
		Server(const Server & src);
		~Server();
		Server & operator=(const Server & rhs);
	protected:
	private:
		std::string _host;
		int _port;
		std::map<std::string, HttpResponse (*)(HttpRequest)> _allowed_methods;
		Locations _allowed_paths;

	friend std::ostream &operator<<(std::ostream &os, const Server &obj);
};

std::ostream &operator<<(std::ostream &os, const Server &obj);
