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
	loc.setAlias("/www/html/index.html");
	loc.setAllowedMethods(location_methodsMap);
	_locations.addLocation(loc);
	loc.clear();

	loc.setURI("/index.html");
	loc.setAlias("/www/html/index.html");
	loc.setAllowedMethods(location_methodsMap);
	_locations.addLocation(loc);
	loc.clear();

	loc.setURI("/favicon.ico");
	loc.setRoot("/www/img/");
	loc.setAllowedMethods(location_methodsMap);
	_locations.addLocation(loc);
	loc.clear();

	loc.setURI("/html/");
	loc.setRoot("/www/");
	loc.setAllowedMethods(location_methodsMap);
	_locations.addLocation(loc);
	loc.clear();

	loc.setURI("/img/");
	loc.setRoot("/www/");
	loc.setAllowedMethods(location_methodsMap);
	_locations.addLocation(loc);
	loc.clear();

	location_methodsMap.clear();
	location_methodsMap.insert("GET");
	location_methodsMap.insert("DELETE");

	loc.setURI("/upload/");
	loc.setRoot("/www/");
	loc.setAllowedMethods(location_methodsMap);
	_locations.addLocation(loc);
	loc.clear();

	location_methodsMap.clear();
	location_methodsMap.insert("GET");
	location_methodsMap.insert("POST");

	loc.setURI("/upload/");
	loc.setRoot("/www/");
	loc.setAllowedMethods(location_methodsMap);
	_locations.addLocation(loc);
	loc.clear();

	loc.setURI("/bin-cgi/");
	loc.setRoot("/www/");
	loc.setAllowedMethods(location_methodsMap);
	loc.isCgi(true);
	_locations.addLocation(loc);
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
		_locations = rhs._locations;
		_errorPages = rhs._errorPages;
		_clients = rhs._clients;
		_methodsMap = rhs._methodsMap;
		_locations = rhs._locations;
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
	for (std::map<std::string, HttpResponse (*)(const HttpRequest &, const LocationContainer &)>::const_iterator it = obj._methodsMap.begin(); it != obj._methodsMap.end(); ++it)
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

const std::map<int, std::string> & Server::getErrorPages() const
{
	return (_errorPages);
}

void Server::setErrorPages(const std::map<int, std::string> & errorPages)
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

const LocationContainer & Server::getLocationContainer(void) const
{
	return (_locations);
}

int Server::getClientMaxBodySize(void) const
{
	return (_maxBodySize);
}

void Server::addLocation(const Location & location)
{
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

		// If the client has a static request, generate a response from a file
		if (client.requestReady() && !isCgi(client.getRequest()))
		{
			HttpResponse response;
			std::cout << "Generating response for client " << client.getIP() << ":" << client.getPort() << " URI " << client.getRequest().get_path() << std::endl; //XXX
			response.generate(client.getRequest(), _locations, _methodsMap);
			client.setResponse(response);
			_clients.erase(it--);
		}
		// If the client has a CGI request, start the CGI process
		else if (_cgiClients.count(ClientInfo(client)) == 0
				&& client.requestReady() && isCgi(client.getRequest()))
			if (startCgi(client) != 0)
			{
				client.setResponse(HttpResponse::error(500)); //TODO return error from _errorPages
				_clients.erase(it--);
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
		std::cout << "killing process  "<< cgiClient._pid << " from client " <<client.getIP()<<":"<<client.getPort() << std::endl; //XXX
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
	try{
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