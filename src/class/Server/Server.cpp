#include "Server.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include "FdHandler.hpp"
#include <fcntl.h>

Server::Server(void)
{
 	_methodsMap["GET"] = &Server::GET;
	_methodsMap["POST"] = &Server::POST;
	_methodsMap["DELETE"] = &Server::DELETE;
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
		_maxBodySize = rhs._maxBodySize;
    	_index = rhs._index;
    	_root = rhs._root;
    	_serverNames = rhs._serverNames;
		_errorPages = rhs._errorPages;
		_clients = rhs._clients;
		_locations = rhs._locations;
		_methodsMap = rhs._methodsMap;
		_activeCGI = rhs._activeCGI;
		_staticResponses = rhs._staticResponses;
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Server &obj)
{
 	os << "port: " << obj._port << std::endl;
	os << "client_max_body_size: " << obj._maxBodySize << std::endl;
	os << "index: " << obj._index << std::endl;
	os << "root: " << obj._root << std::endl;
	os << "server_names: ";
	for (std::set<std::string>::const_iterator it = obj._serverNames.begin(); it != obj._serverNames.end(); ++it)
		os << *it << " ";
	os << std::endl;
	os << "Clients: ";
	for (std::set<Client *>::const_iterator it = obj._clients.begin(); it != obj._clients.end(); ++it)
		os << (*it)->getIP() << ":" << (*it)->getPort() << " ";
	os << std::endl;
	os << "Locations: " << obj._locations << std::endl;
	os << "Available methods: ";
	for (std::map<std::string, void (Server::*)(Client &, const Location &)>::const_iterator it = obj._methodsMap.begin(); it != obj._methodsMap.end(); ++it)
		os << it->first << " ";
	os << std::endl;
	return (os);
}

void Server::setPort(int port)
{
	_port = port;
}

int Server::getPort(void) const
{
	return (_port);
}

void Server::setMaxBodySize(size_t clientMaxBodySize)
{
	_maxBodySize = clientMaxBodySize;
}

size_t Server::getMaxBodySize(void) const
{
	return (_maxBodySize);
}

void Server::setIndex(const std::string & index)
{
	_index = index;
		// update the root of all locations
	size_t len = _locations.size();
	for (size_t i = 0; i < len; ++i)
	{
		Location * loc = const_cast<Location*>(_locations[i]);
		// If the server has an index and the location doesn't, set the index from the server 
		if (not _index.empty() and loc->getIndex().empty())
			loc->setIndex(_index);
	}
}

void Server::setRoot(const std::string & root)
{
	if (!root.empty() && root[0] != '/')
		_root = "/" + root;
	else
		_root = root;
	// update the root of all locations
	size_t len = _locations.size();
	for (size_t i = 0; i < len; ++i)
	{
		Location * loc = const_cast<Location*>(_locations[i]);
		// If the server has a root, merge it with the location's root (this->_root + location->_root)
		if (not _root.empty() and not startsWith(loc->getRoot(), _root))
			loc->setRoot(joinPath(_root, loc->getRoot()));
		// If the server has an alias add the root of the server to it
		if (not _root.empty() and loc->getAlias().size() != 0 and not startsWith(loc->getAlias(), _root))
			loc->setAlias(joinPath(_root, loc->getAlias()) + (back(loc->getAlias()) == '/' ? "/" : ""));
	}
}

const std::string & Server::getRoot(void) const
{
	return (_root);
}

const std::string & Server::getIndex(void) const
{
	return (_index);
}

void Server::addServerName(const std::string & serverName)
{
	_serverNames.insert(serverName);
}

const std::set<std::string> & Server::getServerNames(void) const
{
	return (_serverNames);
}

void Server::setErrorPages(const std::map<int, std::set<std::string> > & errorPages)
{
	_errorPages = errorPages;
}

const std::map<int, std::set<std::string> > & Server::getErrorPages() const
{
	return (_errorPages);
}

void Server::addErrorPage(int error_code, const std::string & errorPage)
{
	if (errorPage.empty())
		return;
	std::string path = cleanPath(errorPage);
	if (!_root.empty())
		path = joinPath(_root, errorPage);
	if (path[0] == '/')
		path = path.substr(1);
	_errorPages[error_code].insert(path);

}

void Server::addClient(Client &client)
{
	_clients.insert(&client);
}

void Server::removeClient(Client &client)
{
	_activeCGI.closeCgi(client);
	if (_staticResponses.count(&client) == 1)
	{
		FdHandler::removeFd(_staticResponses[&client].first);
		_staticResponses.erase(&client);
	}
	if (_clients.count(&client) == 1)
		_clients.erase(&client);
}

bool Server::addLocation(Location location)
{
	// If the server has a root, merge it with the location's root (this->_root + location->_root)
	if (not _root.empty() and not startsWith(location.getRoot(), _root))
		location.setRoot(joinPath(_root, location.getRoot()));
	// If the server has an alias add the root of the server to it
	if (not _root.empty() and location.getAlias().size() != 0 and not startsWith(location.getAlias(), _root))
		location.setAlias(joinPath(_root, location.getAlias()) + (back(location.getAlias()) == '/' ? "/" : ""));
	// If the server has an index and the location doesn't, set the index from the server 
	if (not _index.empty() and location.getIndex().empty())
		location.setIndex(_index);
	return _locations.addLocation(location);
}

const LocationContainer & Server::getLocationContainer(void) const
{
	return (_locations);
}

void Server::errorResponse(Client &client, int error, const std::string & phrase, const std::string & msg)
{
	// If the client is already in the list of clients waiting for a file, return
	if (_staticResponses.count(&client) == 1)
		return ;
	// If there is no error page for the error, return the default error page
	if (_errorPages.count(error) == 0)
		return client.setResponse(HttpResponse::errorResponse(error, phrase, msg));

	// Return the first page that matches a location
	std::set<std::string> errorPages = _errorPages.at(error);
	for (std::set<std::string>::iterator it = errorPages.begin(); it != errorPages.end(); ++it)
	{
		try
		{
			int fd = open(it->c_str(), O_RDONLY);
			if (fd == -1)
				throw std::exception();
			if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
			{
				close(fd);
				break;
			}
			client.getResponse().clear();
			client.getResponse().setStatus(error, phrase);
			client.getResponse().setHeader("Content-Type", extToMime(*it));
			_staticResponses[&client] = std::make_pair(fd, "");
			FdHandler::addFd(fd, POLLIN);
			return;
		}
		catch(const std::exception& e)
		{}
	}
	return client.setResponse(HttpResponse::errorResponse(error, phrase, msg));
}

void Server::loop()
{
	// Iterate the clients to generate and check responses
	// Clients get removed from the list as soon as they have a response or the response started generating
	for (std::set<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		Client &client = **it;
		// If the client has a response ready, remove it from the list of clients
		if (client.responseReady())
			_clients.erase(it--);
		// If the body of the request is too large, return a 413 error
		else if (client.getRequest().getBody().size() > _maxBodySize)
		{
			errorResponse(client, 413, "Request Entity Too Large", "The body of the request is too large");
			_clients.erase(it--);
		}
		// If the client has the request ready, start generating the response
		else if (client.requestReady())
		{
			const HttpRequest &req = client.getRequest();
			const Location * loc = _locations[req.getPath()];
			// Check if it's an allowed method
			if (_methodsMap.count(req.getMethod()) == 0)
				errorResponse(client, 501, "Not implemented", "Method " + req.getMethod() + " is not allowed for this server");
			// Check if the path matches a location. If not, return a 404 error
			else if (!loc)
				errorResponse(client, 404, "Not Found", "The requested URL was not found on this server");
			// If the method is not allowed for the location, return a 405 error
			else if (loc->isAllowedMethod(req.getMethod()) == false)
				errorResponse(client, 405, "Method Not Allowed", "Method " + req.getMethod() + " is not allowed for this location");
			// If the location has a redirection, return it
			else if (loc->hasReturnValue())
				client.setResponse(loc->getReturnResponse());
			else
				(this->*_methodsMap[req.getMethod()])(client, *loc);
			_clients.erase(it--);
		}
	}

	// Iterate the clients that are waiting for a CGI program to finish
	_activeCGI.loop(*this);

	// Iterate the static responses
	for (std::map<Client *, std::pair<int, std::string> >::iterator it = _staticResponses.begin(); it != _staticResponses.end();)
	{
		Client &client = *it->first;
		int fd = it->second.first;
		std::string &fileContent = it->second.second;
		int revents = FdHandler::getRevents(fd);
		if (revents & POLLIN)
		{
			char buff[4096];

			int bytesRead = read(fd, buff, 4096);
			if (bytesRead == -1)
			{
				removeClient(client);
				continue;
			}
			if (bytesRead == 0)
			{
				client.getResponse().setBody(fileContent);
				client.getResponse().responseReady(true);
				removeClient(client);
				continue;
			}
			fileContent.append(buff, bytesRead);
		}
		++it;
	}
}
