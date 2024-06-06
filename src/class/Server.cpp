#include "Server.hpp"
#include "HttpMethods.hpp"
#include "utils.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <poll.h>
#include <cstdlib>
#include "colors.h" //XXX
#include <fcntl.h>

Server::Server(void)
{
	_root = "/www/";
 	_methodsMap["GET"] = GET;
	_methodsMap["POST"] = POST;
	_methodsMap["DELETE"] = DELETE;

	std::set<std::string> location_methodsMap;
	location_methodsMap.insert("GET");

	Location loc;
	loc.setURI("/");
	loc.setAlias("/html/index.html");
	loc.setAllowedMethods(location_methodsMap);
	addLocation(loc);
	loc.clear();

	loc.setURI("/index.html");
	loc.setAlias("/html/index.html");
	loc.setAllowedMethods(location_methodsMap);
	addLocation(loc);
	loc.clear();

	loc.setURI("/favicon.ico");
	loc.setAlias("/img/favicon.ico");
	loc.setAllowedMethods(location_methodsMap);
	addLocation(loc);
	loc.clear();

	loc.setURI("/html/");
	loc.setAllowedMethods(location_methodsMap);
	addLocation(loc);
	loc.clear();

	loc.setURI("/img/");
	loc.setAllowedMethods(location_methodsMap);
	addLocation(loc);
	loc.clear();

	loc.setURI("/css/");
	loc.setAllowedMethods(location_methodsMap);
	addLocation(loc);
	loc.clear();

	location_methodsMap.clear();
	location_methodsMap.insert("GET");
	location_methodsMap.insert("POST");
	location_methodsMap.insert("DELETE");

	loc.setURI("/upload/");
	loc.setAllowedMethods(location_methodsMap);
	addLocation(loc);
	loc.clear();

	location_methodsMap.clear();
	location_methodsMap.insert("GET");
	location_methodsMap.insert("POST");

	loc.setURI("/upload/");
	loc.setAllowedMethods(location_methodsMap);
	addLocation(loc);
	loc.clear();

	loc.setURI("/bin-cgi/");
	loc.setAllowedMethods(location_methodsMap);
	loc.isCgi(true);
	addLocation(loc);
	loc.clear();

	loc.setURI("/google.com");
	loc.setReturnValue(301, "https://www.google.com");
	addLocation(loc);
	loc.clear();

	loc.setURI("/titer");
	loc.setReturnValue(301,"https://x.com/");
	addLocation(loc);
	loc.clear();

	addErrorPage(403,"/error/403.html");

	addErrorPage(404,"/error/404.html");
	addErrorPage(404,"/error/404.htm");

	addErrorPage(405,"/error/405.html");

	addErrorPage(500,"/error/500.html");
	addErrorPage(500,"/error/500.htm");
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
		_cgiClients = rhs._cgiClients;
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
	for (std::map<std::string, HttpResponse (*)(const HttpRequest &, const Server &, const Location &)>::const_iterator it = obj._methodsMap.begin(); it != obj._methodsMap.end(); ++it)
		os << it->first << " ";
	os << std::endl;
	return (os);
}

void Server::setPort(int port)
{
	_port = port;
}

void Server::setMaxBodySize(size_t clientMaxBodySize)
{
	_maxBodySize = clientMaxBodySize;
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
	if (!root.empty() && root[0] != '/')
		_root = "/" + root;
	else
		_root = root;
}

const std::map<int, std::set<std::string> > & Server::getErrorPages() const
{
	return (_errorPages);
}

void Server::setErrorPages(const std::map<int, std::set<std::string> > & errorPages)
{
	_errorPages = errorPages;
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

const std::string & Server::getIndex(void) const
{
	return (_index);
}

void Server::setIndex(const std::string & index)
{
	_index = index;
}

const LocationContainer & Server::getLocationContainer(void) const
{
	return (_locations);
}

int Server::getClientMaxBodySize(void) const
{
	return (_maxBodySize);
}

void Server::addLocation(Location location)
{
	// If the server has a root, merge it with the location's root (this->_root + location->_root)
	if (!_root.empty() && location.getAlias().empty())
		location.setRoot(joinPath(_root, location.getRoot()));
	// If the server has an alias add the root of the server to it
	if (!_root.empty() && !location.getAlias().empty())
		location.setAlias(joinPath(_root, location.getAlias()));
	// If the server has an index and the location doesn't, set the index from the server 
	if (!_index.empty() && location.getIndex().empty())
		location.setIndex(joinPath(_root, location.getIndex()));
	_locations.addLocation(location);
}

void Server::addServerName(const std::string & serverName)
{
	_serverNames.insert(serverName);
}

void Server::loop()
{
	// Generate response for clients that have the request ready
	for (std::set<Client *>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		Client &client = **it;

		// If the client has a response ready, remove it from the list of clients
		if (client.responseReady())
		{
			_clients.erase(it--);
			continue;
		}
		// If the client has the request ready and it is not already generating a response from CGI, start generating the response
		if (client.requestReady() && _cgiClients.count(ClientInfo(client)) == 0)
		{
			const HttpRequest &req = client.getRequest();
			// Get the path of the request
			std::string path = cleanPath(decodeURL(req.get_path().substr(0, req.get_path().find("?"))));
			std::cout << "Generating response for client " << client.getIP() << ":" << client.getPort() << " ("<< req.get_method()<< " " << path << ")" << std::endl; //XXX

			// Check if it's an allowed method
			if (_methodsMap.count(req.get_method()) == 0)
			{
				std::cout << "Method " << req.get_method() << " is not allowed for this server" << std::endl; //XXX
				HttpResponse response = errorResponse(501, "Not implemented", "Method " + req.get_method() + " is not allowed for this server");
				client.setResponse(response);
				_clients.erase(it--);
				continue;
			}
			// Check if the path matches a location, if not return a 404 error
			Location loc;
			if (loc.matchesURI(path))
				loc = _locations[path];
			else
			{
				std::cout << "Location " << path << " not found" << std::endl; //XXX
				client.setResponse(errorResponse(404));
				_clients.erase(it--);
				continue;
			}
			
			// If the method is not allowed for the location, return a 405 error
			if (loc.isAllowedMethod(req.get_method()) == false)
			{
				std::cout << "Method " << req.get_method() << " is not allowed for location " << path << std::endl; //XXX
				HttpResponse response = errorResponse(405, "Method Not Allowed", "Method " + req.get_method() + " is not allowed for this location");
				client.setResponse(response);
				_clients.erase(it--);
				continue;
			}

			// If the location has a redirection, return it
			if (loc.hasReturnValue())
			{
				std::cout << "Location " << path << " has a redirection" << std::endl; //XXX
				client.setResponse(loc.getReturnResponse());
				_clients.erase(it--);
				continue;
			}
			// If the location is not a cgi, return the response
			if (loc.isCgi() == false)
			{
				std::cout << "Location " << path << " is not a CGI" << std::endl; //XXX
				HttpResponse response;
				response = _methodsMap[req.get_method()](req, *this, loc);
				response.setReady(true);
				client.setResponse(response);
				_clients.erase(it--);
				continue;
			}
			// Else is a CGI, start the cgi program
			else if (startCgi(client) != 0)
			{
				client.setResponse(errorResponse(500));
				_clients.erase(it--);
			}
			std::cout << "Location " << path << " is a CGI" << std::endl; //XXX
		}
	}

	// Check if any CGI processes have finished
	for (std::set<ClientInfo>::iterator it = _cgiClients.begin(); it != _cgiClients.end(); ++it)
	{
		int status;
		int pid = waitpid(it->_pid, &status, WNOHANG);
		if (pid > 0) // CGI program has finished
		{
			HttpResponse response;
			response = cgiResponse(*it);
			it->_client->setResponse(response);
			_clients.erase(it->_client);
			_cgiClients.erase(it--);
		}
	}
}

void Server::addClient(Client &client)
{
	_clients.insert(&client);
}

void Server::removeClient(Client &client)
{
	if (_cgiClients.count(ClientInfo(client)) == 1)
	{
		const ClientInfo & cgiClient = *(_cgiClients.find(ClientInfo(client)));
		kill(cgiClient._pid, SIGKILL);
		close(cgiClient._fdOut);
		close(cgiClient._fdIn);
		_cgiClients.erase(ClientInfo(client));
	}
	if (_clients.count(&client) == 1)
	{
		std::cout << "Removing client " <<client.getIP()<<":"<<client.getPort()<<" from server "<< *_serverNames.begin() << std::endl; //XXX
		_clients.erase(&client);
	}
}

Server::ClientInfo::ClientInfo()
{
	_client = NULL;
	_pid = -1;
	_fdOut = -1;
	_fdIn = -1;
}

Server::ClientInfo::ClientInfo(const Client &client, int pid, int fdIn, int fdOut)
{
	_client = const_cast<Client *>(&client);
	_pid = pid;
	_fdIn = fdIn;
	_fdOut = fdOut;
}

bool Server::ClientInfo::operator==(const ClientInfo &rhs) const
{
	return (_client == rhs._client);
}

bool Server::ClientInfo::operator<(const ClientInfo &rhs) const
{
	return (_client < rhs._client);
}

bool Server::isCgi(const HttpRequest &request)
{
	std::string path = request.get_path().substr(0, request.get_path().find("?"));
	path = cleanPath(decodeURL(path));
	try
	{
		const Location & loc = _locations[path];
		return loc.isCgi();
	}
	catch (std::exception & e)
	{
		return false;
	}
	return false;
}

int Server::startCgi(const Client &client)
{
	// Check if the client is already waiting for a CGI program to finish
	if (_cgiClients.count(ClientInfo(client)) == 1)
		return (0);
	// Create a pipe to communicate with the CGI program
	// fdsIn = pipe to read from fork
	// fdsOut = pipe to write to fork
	int fdsIn[2], fdsOut[2];
	if (pipe(fdsIn) != 0 || pipe(fdsOut) != 0)
		return (-1);

	// Fork the process
	int pid = fork();
	if (pid == -1)
	{
		close(fdsIn[0]);
		close(fdsIn[1]);
		close(fdsOut[0]);
		close(fdsOut[1]);
		return (-1);
	}
	// Child process
	if (pid == 0)
	{
		// Redirect stdout of the CGI program to the pipe
		dup2(fdsIn[1],STDOUT_FILENO);
		close(fdsIn[1]);
		close(fdsIn[0]);
		// Redirect stdin of the CGI program to the pipe
		dup2(fdsOut[0],STDIN_FILENO);
		close(fdsOut[1]);
		close(fdsOut[0]);
		// Get the path and environment variables for the CGI program
		char **args = getPath(client.getRequest());
		char **envp = getEnv(client.getRequest());
		if (args == NULL || envp == NULL)
			exit(EXIT_FAILURE);
		// Execute the CGI program
		execve(args[0], args, envp);
		std::cerr << "Server " << *getServerNames().begin() << " failed to execute CGI program" << std::endl; //NOTE msg
		exit(EXIT_FAILURE);
	}
	// Parent process
	// Add the client to the list of clients waiting for the CGI program to finish
	_cgiClients.insert(ClientInfo(client, pid, fdsIn[0], fdsOut[1]));
	close(fdsIn[1]);
	close(fdsOut[0]);
	fcntl(fdsIn[0], F_SETFL, O_NONBLOCK);
	fcntl(fdsOut[1], F_SETFL, O_NONBLOCK);

	return (0);
}

HttpResponse Server::cgiResponse(const ClientInfo &clientInfo) const
{
	//TODO this is only for test
	struct pollfd fds;
	fds.fd = clientInfo._fdIn;
	fds.events = POLLIN;
	if (poll(&fds, 1, 0) == -1)
		return (HttpResponse::error(500)); //TODO return error from _errorPages
	if (fds.revents & POLLIN)
	{
		std::string body;
		char buffer[1024];
		ssize_t bytesRead;
		while ((bytesRead = read(clientInfo._fdIn, buffer, 1024)) > 0)
			body.append(buffer, bytesRead);
		HttpResponse response;
		response.setBody("text/html", body);
		response.setStatus(200, "OK");
		response.setReady(true);
		return (response);
	}
	return (HttpResponse::error(500)); //TODO return error from _errorPages
}

char **Server::getPath(const HttpRequest & req)
{
	char **output = new char*[2];
	if (output == NULL)
		return NULL;
	std::string path = cleanPath(decodeURL(req.get_path().substr(0, req.get_path().find("?"))));
	std::string filename =_locations.getFilename(path);
	if (filename.empty())
	{
		delete[] output;
		return NULL;
	}
	output[0] = strdup(filename.c_str());
	output[1] = NULL;
	return output;
}

char **Server::getEnv(const HttpRequest & req)
{
	// Get the query string
	size_t pos = req.get_path().find("?");
	std::string queryString ="QUERY_STRING=";
	if (pos != std::string::npos && pos != req.get_path().size() - 1)
		queryString += req.get_path().substr(pos + 1);

	// Get the method and uri
	std::string method = std::string("REQUEST_METHOD=") + req.get_method();
	std::string uri = std::string("REQUEST_URI=") + req.get_path();
	if (method.empty() || uri.empty())
		return NULL;

	// Get the headers
	std::list<std::string> headerList;
	std::vector<std::pair<std::string,std::string> > headerVector = req.getHeaders();
	for (size_t i = 0; i < headerVector.size(); ++i)
		headerList.push_back(headerVector[i].first + "=" + headerVector[i].second);

	// Generate the environment variables
	size_t outputSize = headerList.size() + 4;
	char **output = new char*[outputSize];
	if (output == NULL)
		return NULL;
	output[0] = strdup(queryString.c_str());
	output[1] = strdup(method.c_str());
	output[2] = strdup(uri.c_str());
	size_t i = 3;
	for (std::list<std::string>::iterator it = headerList.begin(); it != headerList.end(); ++it)
		output[i++] = strdup(*it);
	output[i] = NULL;

	// Check if any strdup failed
	for (size_t i = 0; i + 1 < outputSize; ++i)
	{
		if (output[i] == NULL)
		{
			for (size_t j = 0; j < outputSize; ++j)
				if (output[j] != NULL)
					delete[] output[j];
			delete[] output;
			return NULL;
		}
	}

	return output;
}

HttpResponse Server::errorResponse(int error, const std::string & phrase, const std::string & msg) const
{
	// If there is no error page for the error, return the default error page
	if (_errorPages.count(error) == 0)
		return HttpResponse::error(error, phrase, msg);

	// Return the first page that matches a location
	std::set<std::string> errorPages = _errorPages.at(error);
	for (std::set<std::string>::iterator it = errorPages.begin(); it != errorPages.end(); ++it)
	{
		try
		{
			HttpResponse response;
			std::string path = *it;
			response.setStatus(error);
			response.setBody(path);
			response.setReady(true);
			return response;
		}
		catch(const std::exception& e)
		{}
	}
	return HttpResponse::error(error, phrase, msg);
}