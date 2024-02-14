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

void Server::setPort(int port)
{
	this->port = port;
}

int Server::getPort(void) const
{
	return (port);
}

std::string Server::getServerName(void) const
{
	return (sv_name);
}

void Server::setServerName(const std::string & serverName)
{
	sv_name = serverName;
}

std::string Server::getHost(void) const
{
	return (host);
}

void Server::setHost(const std::string & host)
{
	this->host = host;
}

std::string Server::getRoot(void) const
{
	return (root);
}

void Server::setRoot(const std::string & root)
{
	this->root = root;
}

std::string Server::getErrorPage(void) const
{
	return (error_page);
}

void Server::setErrorPage(const std::string & errorPage)
{
	error_page = errorPage;
}

std::string Server::getIndex(void) const
{
	return (index);
}

void Server::setIndex(const std::string & index)
{
	this->index = index;
}

std::vector<t_location> Server::getLocations(void) const
{
	return (locations);
}

int Server::getMaxBody(void) const
{
	return (max_body);
}

void Server::setMaxBody(int maxBody)
{
	max_body = maxBody;
}

void Server::addLocation(const t_location & location)
{
	locations.push_back(location);
}
