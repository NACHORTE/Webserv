#include "Listener.hpp"
#include <fcntl.h>
#include "poll.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "defines.h"
#include "utils.hpp"
#include <unistd.h>
#include <cstdio>
#include "debug.hpp"

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
	for (size_t i = 0; i < _clients.size(); ++i)
		close(_clients[i].first.fd);
}

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
				// if readData returns 1, the client is closed and the index is decremented
				i -= readData(_clients[i].first.fd, _clients[i].second);
			// Check if the file descriptor is ready to write
			else if (_clients[i].first.revents & POLLOUT && _clients[i].second.responseReady())
				// if sendData returns 1, the client is closed and the index is decremented
				i -= sendData(_clients[i].first.fd, _clients[i].second);
			// Check if the file descriptor has been closed
			else if (_clients[i].first.revents & (POLLHUP | POLLERR))
				closeConnection(_clients[i--].first.fd);
		}
	}

	// Check if any clients have timed out
	for (size_t i = 0; i < _clients.size(); ++i)
		if (_clients[i].second.timeout())
			closeConnection(i--); // NOTE return timeout error to client before closing

	// Loop through the servers and call their loop function
	for (size_t i = 0; i < _serverVector.size(); ++i)
		_serverVector[i].loop();
}

int Listener::acceptConnection(void)
{
	DEBUG("Accepting connection on port " + int_to_string(_port));
	// Accept the connection
	int newClientFd = accept(_sockfd, NULL, NULL); // NOTE maybe use sockaddr_in instead of NULL for the address
	if (newClientFd < 0)
	{
		std::cout << "[Listener::acceptConnection] Error accepting connection" << std::endl; // NOTE msg
		return 1;
	}

	// Set the new client to non-blocking
	if (fcntl(newClientFd, F_SETFL, O_NONBLOCK) < 0)
		std::cout << "[Listener::acceptConnection] Error fcntl" << std::endl; // NOTE msg

	// Add the new client to the list of clients
	struct pollfd pfd;
	pfd.fd = newClientFd;
	pfd.events = POLLIN | POLLOUT | POLLHUP | POLLERR;
	_clients.push_back(std::make_pair(pfd, Client()));

	return 0;
}

int Listener::readData(int fd, Client &client)
{
	DEBUG("Reading data from client on port " + int_to_string(_port));
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

	// Add the data to the client's buffer
	client.addData(std::string(buffer,bytesRead));

	// If the request is ready, send it to a server
	if (client.requestReady())
		sendToServer(client);

	return 0;
}

int Listener::sendData(int fd, Client &client)
{
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

int Listener::closeConnection(int fd)
{
	// Find the client in the list of clients
	size_t clientIndex = 0;
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i].first.fd == fd)
		{
			clientIndex = i;
			break;
		}
		if (i == _clients.size() - 1)
		{
			std::cout << "[Listener::closeConnection] Client not found" << std::endl; // NOTE msg
			return 1;
		}
	}

	// Delete the pointer to the client from the servers (try to delete from all just in case)
	for (size_t i = 0; i < _serverVector.size(); ++i)
		_serverVector[i].removeClient(_clients[clientIndex].second);

	// Close the connection
	close(fd);

	// Remove the client from the list of clients
	_clients.erase(_clients.begin() + clientIndex);

	return 0;
}

void Listener::sendToServer(Client &client)
{
	std::string hostname = client.getHost();
	DEBUG ("Sending request to server" + hostname);
	// If the server exists, add the client to the server
	if (_serverMap.count(hostname) == 1)
		_serverMap[hostname]->addClient(client);
	// Else send it to the default server
	else
		_serverVector[0].addClient(client);
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
