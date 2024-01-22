#include "Server.hpp"
#include "HttpMethods.hpp"
#include "utils.hpp"

Server::Server(void)
{
	_allowed_methods["GET"] = GET;
	_allowed_methods["POST"] = POST;
	_allowed_methods["DELETE"] = DELETE;

	std::set<std::string> location_allowed_methods;
	location_allowed_methods.insert("GET");
	_allowed_paths.addLocation("/", true, "/html/index.html", location_allowed_methods);
	_allowed_paths.addLocation("/index.html", true, "/html/index.html", location_allowed_methods);
	_allowed_paths.addLocation("/favicon.ico", true, "/img/favicon.ico", location_allowed_methods);
	_allowed_paths.addLocation("/html/", false, "", location_allowed_methods);
	_allowed_paths.addLocation("/img/", false, "", location_allowed_methods);

	location_allowed_methods.clear();
	location_allowed_methods.insert("GET");
	location_allowed_methods.insert("DELETE");
	_allowed_paths.addLocation("/upload/", false, "", location_allowed_methods);

	location_allowed_methods.clear();
	location_allowed_methods.insert("GET");
	location_allowed_methods.insert("POST");
	_allowed_paths.addLocation("/upload", true, "", location_allowed_methods);

	location_allowed_methods.clear();
	location_allowed_methods.insert("POST");
	location_allowed_methods.insert("GET");
	_allowed_paths.addLocation("/bin-cgi/", false, "", location_allowed_methods);

	//std::cout << "allowed paths:" << std::endl;
	//std::cout << _allowed_paths << std::endl;
	//std::cout << "allowed methods:" << std::endl;
	//for (std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)>::iterator it = _allowed_methods.begin(); it != _allowed_methods.end(); ++it)
	//	std::cout << it->first << std::endl;
}

Server::Server(const Server & src)
{
	*this = src;
}

Server::~Server()
{
}

Server &Server::operator=(const Server &rhs)
{
	if (this != &rhs)
	{
		// copy
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Server &obj)
{
	(void)obj;
	return (os);
}
