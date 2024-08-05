#include "Listener.hpp"
#include "defines.h"
#include "utils.hpp"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>
#include <arpa/inet.h>
#include <poll.h>

Listener::Listener(int port) : _port(port)
{
	// Set up the server address to listen any address and the specified port
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(_port);

	// Create the socket
	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("[Listener::init_socket] Error creating socket with port " + intToString(_port));
	// Enable the socket to begin listening
	if (bind(_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		throw std::runtime_error("[Listener::init_socket] Error binding socket with port " + intToString(_port));
	if (listen(_sockfd, BACKLOG) < 0)
		throw std::runtime_error("[Listener::init_socket] Error listening socket with port " + intToString(_port));
	// Set the socket to non-blocking so accept will not block
	if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("[Listener::init_socket] Error fcntl");

	// Add the server socket to the list of clients
	struct pollfd pfd;
	pfd.fd = _sockfd;
	pfd.events = POLLIN;
	_pollfds.push_back(pfd);
	_clients.push_back(Client());
}

Listener::Listener(const Listener &src)
{
	*this = src;
}

Listener::~Listener()
{}

void Listener::addServer(const Server & server)
{
	// Get the hostnames of the new server
	const std::set<std::string> & serverNames = server.getServerNames();

	// If there is no server name, add it to the front.
	// If there is already a server with no name, throw an error
	// Don't add the nameless server to the server map
	if (serverNames.size() == 0)
	{
		if (not _serverList.empty() and _serverList.front().getServerNames().size() == 0)
			throw std::runtime_error("[ERROR] Listener " + intToString(_port)
				+ " has a duplicate nameless server");
		_serverList.push_front(server);
		return;
	}

	// Check if there is a server with the same hostname
	for (std::set<std::string>::const_iterator it = serverNames.begin();
			it != serverNames.end();++it)
		if (_serverMap.count(*it) == 1)
			throw std::runtime_error("[ERROR] Listener " + intToString(_port)
				+ " has a duplicate hostname: " + *it);

	// Add the server to the vector of servers
	_serverList.push_back(server);

	// Add the hostnames of the server to the map of servers
	for (std::set<std::string>::const_iterator it = serverNames.begin();
			it != serverNames.end(); ++it)
		_serverMap[*it] = &_serverList.back();
}

int Listener::getPort(void) const
{
	return (_port);
}

void Listener::loop()
{
	// Check if there are any file descriptors ready to read or write
	if (poll(&_pollfds[0], _pollfds.size(), POLL_TIMEOUT) > 0)
	{
		// Find the file descriptors that are ready
		std::list<Client>::iterator it = _clients.begin();
		for (size_t i = 0; i < _clients.size(); ++i , ++it)
		{
			// If the server is ready to read, accept the connection
			// Check if the file descriptor has been closed
			if (_pollfds[i].revents & POLLIN && _pollfds[i].fd == _sockfd)
				acceptConnection();
			else if (_pollfds[i].revents & (POLLHUP | POLLERR))
			{
				closeConnection(_pollfds[i--].fd);
				--it;
			}
			// If the client is ready to read, read the data
			else if (_pollfds[i].revents & POLLIN)
			{
				// if readData returns 1, the client is closed and the index is decremented
				if (readData(_pollfds[i].fd, *it))
					(--i , --it);
			}
			// Check if the file descriptor is ready to write
			else if (_pollfds[i].revents & POLLOUT && it->responseReady())
			{
				// if sendData returns 1, the client is closed and the index is decremented
				if(sendData(_pollfds[i].fd, *it))
					(--i , --it);
			}
		}
	}

	// Check if any clients have timed out (skip the first client bc its the listener)
	size_t i = 1;
	for (std::list<Client>::iterator it = ++_clients.begin(); it != _clients.end(); ++it, ++i)
	{
		if (it->timeout() and it->responseReady())
			closeConnection(_pollfds[i--].fd);
		else if (it->timeout() and not it->responseReady())
		{
			DEBUG("Listener " << _port << ", Client " << it->getIP() << ":" << it->getPort() << " timed out");
			it->setResponse(errorResponse(*it, 408, "Request Timeout", "Client timed out"));
		}
	}

	// Loop through the servers and call their loop function
	for (std::list<Server>::iterator it = _serverList.begin(); it != _serverList.end(); ++it)
		it->loop();
}

void Listener::closeFds()
{
	// Close all the clients
	for (size_t i = 1; i < _pollfds.size(); ++i)
		closeConnection(i);
	// Close the server
	close(_sockfd);
}

Listener &Listener::operator=(const Listener &src)
{
	if (this != &src)
	{
		_port = src._port;
		_sockfd = src._sockfd;
		_serverList = src._serverList;
		for (std::list<Server>::iterator it = _serverList.begin(); it != _serverList.end(); ++it)
			for (std::set<std::string>::const_iterator it2 = it->getServerNames().begin();
				it2 != it->getServerNames().end(); ++it2)
				_serverMap[*it2] = &(*it);
		_pollfds = src._pollfds;
		_clients = src._clients;
	}
	return (*this);
}

int Listener::acceptConnection(void)
{
	// Accept the connection
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int newClientFd = accept(_sockfd, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
	if (newClientFd < 0)
	{
		DEBUG("Listener " << _port << ", Error accepting connection");
		return 1;
	}

	// Set the new client to non-blocking
	if (fcntl(newClientFd, F_SETFL, O_NONBLOCK) < 0)
		DEBUG("Listener " << _port << ", Error fcntl");

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddr.sin_port);
    std::string clientIPAddress(clientIP);

	// Add the new client to the list of clients
	struct pollfd pfd;
	pfd.fd = newClientFd;
	pfd.events = POLLIN | POLLOUT | POLLHUP | POLLERR;
	_pollfds.push_back(pfd);
	_clients.push_back(Client(clientIP, clientPort));

	DEBUG("Listener " << _port << ", Client " << clientIPAddress << ":" << clientPort << ", connected");
	return 0;
}

int Listener::readData(int fd, Client &client)
{
	// Read the data from the client
	char buffer[BUFSIZ];
	int bytesRead = read(fd, buffer, BUFSIZ);
	// If there is an error, close the connection
	if (bytesRead < 0)
	{
		DEBUG("Listener " << _port << ", Client " << client.getIP() << ":" << client.getPort() << ", Error reading data");
		client.setResponse(errorResponse(client, 500, "Internal_serv_error", "Couldn't read data from client"));
		return 0;
	}
	if (bytesRead == 0)
		return 0;

	DEBUG("Listener " << _port << ", Client " << client.getIP() << ":" << client.getPort() << ", Read " << bytesRead << " bytes");
	// Add the data to the client's buffer
	client.addData(std::string(buffer,bytesRead));
	// If there is an error, generate an error response that will be sent in the next sendData
	if (client.error())
	{
		client.setResponse(errorResponse(client, 400, "Bad Request", "Error parsing request"));
		return 0;
	}

	// send the client to a server if there is an error so it generates a response
	if (client.requestReady())
		sendToServer(client);
	return 0;
}

int Listener::sendData(int fd, Client &client)
{
	std::string response = client.getResponse().to_string();

	// Send the data to the client
	int bytesSent = write(fd, response.c_str(), response.size());
	if (bytesSent <= 0)
	{
		DEBUG("Listener " << _port << ", Client " << client.getIP() << ":" << client.getPort() << ", Error sending data");
		closeConnection(fd);
		return 1;
	}
			DEBUG("Listener " << _port << ", Client " << client.getIP() << ":" << client.getPort()
			<< ", ("<< client.getRequest().getMethod() << " " << client.getRequest().getPath()
			<< "), (" << client.getResponse().getStatusCode()
			<< ") Sent " << bytesSent << " bytes");

	// If keep-alive is set pop the request and wait for another one
	if (client.keepAlive() and not client.error())
	{
		DEBUG("Listener " << _port << ", Client " << client.getIP() << ":" << client.getPort() << ", not disconnecting Keep-alive set");
		client.popRequest();
	}
	// Close the connection otherwise
	else
	{
		closeConnection(fd);
		return 1;
	}

	// If there is another request ready, send it to a server
	if (client.requestReady())
		sendToServer(client);

	return 0;
}

int Listener::closeConnection(int fd)
{
	// Find the index of the client
	size_t clientIndex = 0;
	for (clientIndex = 0; clientIndex < _pollfds.size(); ++clientIndex)
		if (_pollfds[clientIndex].fd == fd)
			break;
	// Don't allow to remove the listener
	if (clientIndex == 0)
	{
		DEBUG("Listener " << _port << ", Can't close the listener socket");
		return 1;
	}
	// Get the client from the list of clients
	std::list<Client>::iterator clientIt = _clients.begin();
	std::advance(clientIt, clientIndex);

	DEBUG("Listener " << _port << ", Client " << clientIt->getIP() << ":" << clientIt->getPort() << ", disconnected");

	// Delete the pointer to the client from the servers (try to delete from all just in case)
	for (std::list<Server>::iterator it = _serverList.begin(); it != _serverList.end(); ++it)
		it->removeClient(*clientIt);

	// Close the connection
	close(_pollfds[clientIndex].fd);

	// Remove the client from the list of clients
	_pollfds.erase(_pollfds.begin() + clientIndex);
	_clients.erase(clientIt);

	return 0;
}

void Listener::sendToServer(Client &client)
{
	std::string hostname = client.getHost().substr(0, client.getHost().find(':'));
	// If the server exists, add the client to the server
	if (_serverMap.count(hostname) == 1)
	{
		DEBUG("Listener " << _port << ", Client " << client.getIP() << ":" << client.getPort()
			<< ", ("<< client.getRequest().getMethod() << " " << client.getRequest().getPath()
			<< ") forwarded to server " << hostname);
		_serverMap[hostname]->addClient(client);
	}
	// Else send it to the default server
	else
	{
		DEBUG("Listener " << _port << ", Client " << client.getIP() << ":" << client.getPort()
			<< ", ("<< client.getRequest().getMethod() << " " << client.getRequest().getPath()
			<< ") forwarded to default server");
		_serverList.front().addClient(client);
	}
}

std::ostream &operator<<(std::ostream &os, const Listener &obj)
{
	os << "Listener (port " << obj._port << ") (socket " << obj._sockfd << ")" << std::endl;
	os << "   Amount of servers: " << obj._serverList.size() << std::endl;
	// print all servers hostnames
	size_t i = 0;
	for (std::list<Server>::const_iterator it = obj._serverList.begin(); it != obj._serverList.end(); ++it)
	{
		os << "\t" << ++i << ". SERVER_NAME:";
		std::set<std::string> serverNames = it->getServerNames();
		for (std::set<std::string>::const_iterator it2 = serverNames.begin();
			it2 != serverNames.end(); ++it2)
			os << " " << *it2;
		os << std::endl;
		const LocationContainer & cont = it->getLocationContainer();
		os << "\t   " << "Amount of locations: " << cont.size() << std::endl;
		for (size_t i = 0; i < cont.size(); ++i)
		{
			os << "\t\t"<< i+1 << ". " << cont[i]->getURI();
			const std::set<std::string> methods =  cont[i]->getAllowedMethods();
			os << " allowed methods:";
			if (methods.empty())
				os << " all";
			else
				for (std::set<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it)
					os << " " <<*it;
			os << std::endl;
		}
	}
	return (os);
}

HttpResponse Listener::errorResponse(Client & client, int errorCode, const std::string & phrase, const std::string & msg)
{
	// Set the error code in the client so when the response is sent, the client is closed
	client.error(true);

	// Get the host of the request if possible
	if (client.getRequestCount() == 0)
		return _serverList.front().errorResponse(errorCode, phrase, msg);
	std::vector<std::string> host = client.getRequest().getHeader("Host");
	std::string hostname = host.size() > 0 ? host[0] : "";

	// If the server exists, add the client to the server
	if (_serverMap.count(hostname) == 1)
		return _serverMap.at(hostname)->errorResponse(errorCode, phrase, msg);
	// Else send it to the default server
	return _serverList.front().errorResponse(errorCode, phrase, msg);
}