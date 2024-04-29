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
}

Server::Server(const Server & src)
{
	*this = src;
}

Server::~Server()
{}

Server &Server::operator=(const Server &rhs)
{
	if (this != &rhs)
	{
		_port = rhs._port;
		_clientMaxBodySize = rhs._clientMaxBodySize;
    	_index = rhs._index;
    	_root = rhs._root;
    	_serverNames = rhs._serverNames;
		_locations = rhs._locations;
		_errorPages = rhs._errorPages;
		_clients = rhs._clients;
		_allowed_methods = rhs._allowed_methods;
		_allowed_paths = rhs._allowed_paths;
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Server &obj)
{
 	os << "port: " << obj._port << std::endl;
	os << "client_max_body_size: " << obj._clientMaxBodySize << std::endl;
	os << "index: " << obj._index << std::endl;
	os << "root: " << obj._root << std::endl;
	os << "server_names: ";
	for (std::set<std::string>::const_iterator it = obj._serverNames.begin(); it != obj._serverNames.end(); ++it)
		os << *it << " ";
	os << std::endl;
	return (os);
}

void Server::setPort(int port)
{
	_port = port;
}

void Server::setClientMaxBodySize(size_t clientMaxBodySize)
{
	_clientMaxBodySize = clientMaxBodySize;
}

int Server::getPort(void) const
{
	return (_port);
}

const std::set<std::string> & Server::getServerNames(void) const
{
	return (_serverNames);
}

const std::string & Server::getRoot(void) const
{
	return (_root);
}

void Server::setRoot(const std::string & root)
{
	_root = root;
}

const std::vector<std::string> & Server::getErrorPages() const
{
	return (_errorPages);
}

void Server::setErrorPages(const std::vector<std::string> & errorPages)
{
	_errorPages = errorPages;
}

const std::string & Server::getIndex(void) const
{
	return (_index);
}

void Server::setIndex(const std::string & index)
{
	_index = index;
}

const std::vector<Location> & Server::getLocations(void) const
{
	return (_locations);
}

int Server::getClientMaxBodySize(void) const
{
	return (_clientMaxBodySize);
}

void Server::addLocation(const Location & location)
{
	_locations.push_back(location);
}

void Server::addServerName(const std::string & serverName)
{
	_serverNames.insert(serverName);
}

void Server::loop()
{
	std::set<Client *>::iterator it;

	std::cout << "Server::loop::clients:";
	for (std::set<Client *>::iterator it2 = _clients.begin(); it2 != _clients.end(); it2++)
		std::cout << " " << *it2;
	std::cout << std::endl;
	
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		Client &client = **it;
		if (client.requestReady() && !client.responseReady())
		{
			HttpResponse response;
			response.generate(client.getRequest(), _allowed_paths, _allowed_methods);
			client.setResponse(response);
			std::cout << client << std::endl;
		}
	}
}

void Server::addClient(Client &client)
{
	_clients.insert(&client);
}

void Server::removeClient(Client &client)
{
	if (_clients.count(&client) == 1)
		_clients.erase(&client);
}
