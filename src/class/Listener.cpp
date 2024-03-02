#include "Listener.hpp"
#include <fcntl.h>
#include "poll.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "defines.h"
#include "utils.hpp"
#include <unistd.h>
#include <cstdio>

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
	_clients.push_back(std::make_pair(pfd, Client()));
}

Listener::~Listener()
{
	closeListener();
}

void Listener::addServer(const Server & server)
{
	// Get the hostnames of the new server
	const std::set<std::string> serverNames = server.getServerNames();

	// Check if there is a server with the same hostname
	for (std::set<std::string>::const_iterator it = serverNames.begin();
		it != serverNames.end(); ++it)
		if (_serverMap.count(*it) == 1)
			throw std::runtime_error("[ERROR] Listener " + std::to_string(_port) + " has a duplicate hostname: " + *it);

	// Add the server to the vector of servers
	_serverVector.push_back(server);

	// Add the hostnames of the server to the map of servers
	for (std::set<std::string>::const_iterator it = serverNames.begin();
		it != serverNames.end(); ++it)
		_serverMap[*it] = &_serverVector[_serverVector.size() - 1];
}

Server &Listener::getServer(const std::string &hostname) const
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
	if (poll(&_clients[0].first, _clients.size(), POLL_TIMEOUT) > 0)
	{
		// Find the file descriptors that are ready
		for (size_t i = 0; i < _clients.size(); ++i)
		{
			// If the server is ready to read, accept the connection
			if (_clients[i].first.revents & POLLIN && _clients[i].first.fd == _sockfd)
				acceptConnection();
			// If the client is ready to read, read the data
			else if (_clients[i].first.revents & POLLIN)
				readData(_clients[i].first.fd, _clients[i].second);
			// Check if the file descriptor is ready to write
			else if (_clients[i].first.revents & POLLOUT && _clients[i].second.responseReady())
				sendData(_clients[i].first.fd, _clients[i].second);
			// Check if the file descriptor has been closed
			else if (_clients[i].first.revents & (POLLHUP | POLLERR))
				closeConnection(i--);
		}
	}

	// Loop through the servers and call their loop function
	for (size_t i = 0; i < _serverVector.size(); ++i)
		_serverVector[i].loop();
}

void Listener::closeListener(void)
{
	for (size_t i = 0; i < _clients.size(); ++i)
		close(_clients[i].first.fd);
}

void Listener::readData(int fd, Client &client)
{
	// Read the data from the client
	char buffer[BUFSIZ];
	int bytesRead = read(fd, buffer, BUFSIZ);
	if (bytesRead < 0) // NOTE don't use throw, use std::cerr and disconect the client
		throw std::runtime_error("[Listener::readData] Error reading data from client");

	// Add the data to the client's buffer
	client.addData(buffer, bytesRead);
	std::string hostname = client.getHost();
	if (!hostname.empty())
	{
		// Add the client to the server
		if (_serverMap.count(client.getHost()) == 1)
			_serverMap[hostname].addClient(&client);
		else // TODO do something here
			std::cerr << "[Listener::readData] Hostname not found: " << hostname << std::endl;
	}
}

void Listener::sendData(int fd, Client &client)
{
	// Send the data to the client
	int bytesSent = write(fd, client.getResponse().c_str(), client.getResponse().size());
	if (bytesSent < 0) // NOTE don't use throw, use std::cerr and disconect the client
		throw std::runtime_error("[Listener::sendData] Error sending data to client");

	// Remove the data from the client's buffer
	client.removeData(bytesSent);
}

void Listener::acceptConnection(void)
{
	// Accept the connection
	int newClientFd = accept(_sockfd, NULL, NULL); // NOTE maybe use sockaddr_in instead of NULL for the address
	if (newClientFd < 0) // NOTE don't use throw, use std::cerr
		throw std::runtime_error("[Listener::acceptConnection] Error accepting connection");

	// Set the new client to non-blocking
	if (fcntl(newClientFd, F_SETFL, O_NONBLOCK) < 0)
		std::cout << "[Listener::acceptConnection] Error fcntl" << std::endl;

	// Add the new client to the list of clients
	struct pollfd pfd;
	pfd.fd = newClientFd;
	pfd.events = POLLIN | POLLOUT | POLLHUP | POLLERR;
	_clients.push_back(std::make_pair(pfd, Client()));
}

void Listener::closeConnection(int clientIndex)
{
	// Delete the pointer to the client from the server (try to delete from all just in case)
	for (size_t i = 0; i < _serverVector.size(); ++i)
		_serverVector[i].removeClient(&_clients[clientIndex].second);

	// Close the connection
	close(_clients[clientIndex].first.fd);

	// Remove the client from the list of clients
	_clients.erase(_clients.begin() + clientIndex);
}

std::ostream &operator<<(std::ostream &os, const Listener &obj)
{
	os << "Listener port (" << obj._port << ")" << std::endl;
	os << "\tAmount of servers: " << obj._serverVector.size() << std::endl;
	// print all servers hostnames
	for (size_t i = 0; i < obj._serverVector.size(); ++i)
	{
		os << "\t\tServer " << i++ << ":";
		std::set<std::string> serverNames = obj._serverVector[i].getServerNames();
		for (std::set<std::string>::const_iterator it2 = serverNames.begin();
			it2 != serverNames.end(); ++it2)
			os << " " << *it2;
		os << std::endl;
	}
	os << "\tAmount of clients: " << obj._clients.size() << std::endl;
	return (os);
}
