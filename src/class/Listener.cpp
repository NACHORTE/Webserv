#include "Listener.hpp"
#include <fcntl.h>
#include "poll.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "defines.h"
#include "utils.hpp"
#include <unistd.h>
#include <cstdio>
#include <arpa/inet.h>

Listener::Listener(int port) : _port(port)
{
	// Set up the server address to listen any address and the specified port
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(_port);

	// Create the socket
	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("[Listener::init_socket] Error creating socket with port" + int_to_string(_port));
	std::cout << "Listener " << _port << " socket " << _sockfd << std::endl;
	
	// Enable the socket to begin listening
	if (bind(_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		throw std::runtime_error("[Listener::init_socket] Error binding socket with port" + int_to_string(_port));
	if (listen(_sockfd, BACKLOG) < 0)
		throw std::runtime_error("[Listener::init_socket] Error listening socket with port " + int_to_string(_port));

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
	const std::set<std::string> serverNames = server.getServerNames();

	// Check if there is a server with the same hostname
	for (std::set<std::string>::const_iterator it = serverNames.begin();
		it != serverNames.end(); ++it)
		if (_serverMap.count(*it) == 1)
			throw std::runtime_error("[ERROR] Listener " + int_to_string(_port) + " has a duplicate hostname: " + *it);

	// Add the server to the vector of servers
	_serverVector.push_back(server);

	// Add the hostnames of the server to the map of servers
	for (std::set<std::string>::const_iterator it = serverNames.begin();
		it != serverNames.end(); ++it)
		_serverMap[*it] = &_serverVector[_serverVector.size() - 1];
}

Server &Listener::getServer(const std::string &hostname) const // XXX UNUSED ?
{
	if (_serverMap.count(hostname) == 1)
		return (*_serverMap.at(hostname));
	throw std::runtime_error("[Listener::getServer] Server not found for hostname: " + hostname);
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
		for (size_t i = 0; i < _clients.size(); ++i)
		{
			// If the server is ready to read, accept the connection
			if (_pollfds[i].revents & POLLIN && _pollfds[i].fd == _sockfd)
				acceptConnection();
			// If the client is ready to read, read the data
			else if (_pollfds[i].revents & POLLIN)
				// if readData returns 1, the client is closed and the index is decremented
				i -= readData(_pollfds[i].fd, _clients[i]);
			// Check if the file descriptor is ready to write
			else if (_pollfds[i].revents & POLLOUT && _clients[i].responseReady())
				// if sendData returns 1, the client is closed and the index is decremented
				i -= sendData(_pollfds[i].fd, _clients[i]);
			// Check if the file descriptor has been closed
			else if (_pollfds[i].revents & (POLLHUP | POLLERR))
				closeConnection(_pollfds[i--].fd);
		}
	}

	// Check if any clients have timed out (skip the first client bc its the listener)
	for (size_t i = 1; i < _clients.size(); ++i)
		if (_clients[i].timeout())
			closeConnection(i--); // NOTE return timeout error to client before closing


	// Loop through the servers and call their loop function
	for (size_t i = 0; i < _serverVector.size(); ++i)
		_serverVector[i].loop();
}

void Listener::closeFds()
{
	// Close all the clients
	for (size_t i = 0; i < _clients.size(); ++i)
		closeConnection(i);
}

Listener &Listener::operator=(const Listener &src)
{
	if (this != &src)
	{
		_port = src._port;
		_sockfd = src._sockfd;
		_serverVector = src._serverVector;
		_serverMap = src._serverMap;
		_pollfds = src._pollfds;
		_clients = src._clients;
	}
	return (*this);
}

int Listener::acceptConnection(void)
{
	std::cout << "Accepting connection" << std::endl; //XXX
	// Accept the connection
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int newClientFd = accept(_sockfd, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen); // NOTE maybe use sockaddr_in instead of NULL for the address
	if (newClientFd < 0)
	{
		std::cout << "[Listener::acceptConnection] Error accepting connection" << std::endl; // NOTE msg
		return 1;
	}

	// Set the new client to non-blocking
	if (fcntl(newClientFd, F_SETFL, O_NONBLOCK) < 0)
		std::cout << "[Listener::acceptConnection] Error fcntl" << std::endl; // NOTE msg

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
    int clientPort = ntohs(clientAddr.sin_port);

    std::string clientIPAddress(clientIP); // Convertir a std::string
	// Add the new client to the list of clients
	struct pollfd pfd;
	pfd.fd = newClientFd;
	pfd.events = POLLIN | POLLOUT | POLLHUP | POLLERR;
	_pollfds.push_back(pfd);
	_clients.push_back(Client(clientIP, clientPort));

	std::cout << *this << std::endl;
	return 0;
}

int Listener::readData(int fd, Client &client)
{
	//std::cout << *this << std::endl;
	// Read the data from the client
	char buffer[BUFSIZ];
	int bytesRead = read(fd, buffer, BUFSIZ);
	// If there is an error, close the connection
	if (bytesRead < 0)
	{
		// NOTE send internal server error to client before closing
		closeConnection(fd);
		return 1;
	}
	if ( bytesRead == 0) // NOTE fix POLLIN always true
		return 0;

	std::cout << "Reading data from " << client.getIP() << ":" << client.getPort() << std::endl; //XXX
	std::cout << "Read " << bytesRead << " bytes" << std::endl; //XXX

	// Add the data to the client's buffer
	client.addData(std::string(buffer,bytesRead));
	// If the request is ready, send it to a server
	if (client.requestReady())
		sendToServer(client);
	return 0;
}

int Listener::sendData(int fd, Client &client)
{
	std::cout << "Sending data" << std::endl;
	std::cout << *this << std::endl;
	// NOTE send everything for now, maybe send in chunks later
	// Get the response chunk from the client
	std::string response = client.popResponse(/*NOTE BUFSIZ*/);
	// Send the data to the client
	int bytesSent = write(fd, response.c_str(), response.size());
	if (bytesSent < 0)
	{
		std::cout << "[Listener::sendData] Error sending data" << std::endl; // NOTE msg
		closeConnection(fd);
		return 1;
	}

	// Pop the request if the whole response has been sent
	if (client.getResponse().size() == 0)
	{
		// Close the connection if the client is not keep-alive
		if (!client.keepAlive())
		{
			closeConnection(fd);
			return 1;
		}
		// Else pop the request and wait for another one
		else
			client.popRequest();
		// If there is another request ready, send it to a server
		if (client.requestReady())
			sendToServer(client);
	}

	return 0;
}

int Listener::closeConnection(int clientIndex)
{
	std::cout << "Closing connection at index "<< clientIndex << ", fd: " << _pollfds[clientIndex].fd << std::endl;
	std::cout << *this << std::endl;
	// Don't allow to remove the listener
	if (clientIndex == 0)
	{
		std::cout << "[Listener::closeConnection] Can't close client 0 (listener socket)" << std::endl;
		return 1;
	}

	// Delete the pointer to the client from the servers (try to delete from all just in case)
	for (size_t i = 0; i < _serverVector.size(); ++i)
		_serverVector[i].removeClient(_clients[clientIndex]);

	// Close the connection
	close(_pollfds[clientIndex].fd);

	// Remove the client from the list of clients
	_pollfds.erase(_pollfds.begin() + clientIndex);
	_clients.erase(_clients.begin() + clientIndex);

	return 0;
}

void Listener::sendToServer(Client &client)
{
	std::string hostname = client.getHost();
	// If the server exists, add the client to the server
	if (_serverMap.count(hostname) == 1)
		_serverMap[hostname]->addClient(client);
	// Else send it to the default server
	else
		_serverVector[0].addClient(client);
}

std::ostream &operator<<(std::ostream &os, const Listener &obj)
{
	os << "Listener (port " << obj._port << ") (socket " << obj._sockfd << ")" << std::endl;
	os << "\tAmount of servers: " << obj._serverVector.size() << std::endl;
	// print all servers hostnames
	for (size_t i = 0; i < obj._serverVector.size(); ++i)
	{
		os << "\t\tServer " << i + 1 << ":";
		std::set<std::string> serverNames = obj._serverVector[i].getServerNames();
		for (std::set<std::string>::const_iterator it2 = serverNames.begin();
			it2 != serverNames.end(); ++it2)
			os << " " << *it2;
		os << std::endl;
	}
	os << "\tAmount of clients: " << obj._clients.size() - 1 << std::endl;
	for (size_t i = 1; i < obj._clients.size(); ++i)
	{
		os << "\t\tClient " << i << " (IP " << obj._clients[i].getIP() << ":" << obj._clients[i].getPort();
		os << ") (fd " << obj._pollfds[i].fd << ") (request count " << obj._clients[i].getRequestCount() << (obj._clients[i].requestReady()?" ready":" not ready")<< ")" << std::endl;
	}
	return (os);
}
