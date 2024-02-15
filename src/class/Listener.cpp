#include "Listener.hpp"
#include <fcntl.h>

Listener::Listener(int port) : _port(port)
{
	if (init_socket() != 0)
		throw std::exception();
}

Listener::~Listener()
{
}

void Listener::addServer(const Server &server)
{
	_servers.push_back(server);
}

int Listener::getPort(void) const
{
	return (_port);
}

void Listener::loop()
{
	// Accept new connections and read/write data from existing connections
	int newClientFd = accept(_sockfd, NULL, NULL); // NOTE maybe use sockaddr_in
	if (newClientFd > 0)
	{
		// Set the new client to non-blocking
		if (fcntl(newClientFd, F_SETFL, O_NONBLOCK) < 0)
			std::cout << "[Listener::loop] Error fcntl" << std::endl;
		// Add the new client to the list of clients
		struct pollfd pfd;
		pfd.fd = newClientFd;
		pfd.events = POLLIN | POLLOUT | POLLHUP; // POLLHUP is for when the client closes the connection
		_clients.push_back(std::make_pair(pfd, Client()));
	}

	// Check if there are any file descriptors ready to read or write
	if (poll(&_clients[0].first, _clients.size(), 0) > 0)
	{
		// Find the file descriptos that are ready
		for (size_t i = 0; i < _clients.size(); i++)
		{
			// Check if the file descriptor is ready to read
			if (_clients[i].first.revents & POLLIN)
			{
				// If the file descriptor is the server socket, then there is a new connection
				if (_clients[i].first.fd == _sockfd)
					acceptConnection();
				// Otherwise, it is an existing connection
				else
					readData(_clients[i].first.fd);
			}
			// Check if the file descriptor is ready to write
			if (_clients[i].first.revents & POLLOUT)
				sendData(_clients[i].first.fd);
			// Check if the file descriptor has been closed
			if (_clients[i].first.revents & POLLHUP) {
				closeConnection(_clients[i].first.fd);
				// Remove the client from the list of clients
				_clients.erase(_clients.begin() + i);
				// Decrement i to account for the removed client
				i--;
			}
		}
	}
}

Listener &Listener::operator=(const Listener &rhs)
{
	if (this != &rhs)
	{
		// copy
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Listener &obj)
{
	(void)obj;
	return (os);
}

int Listener::init_socket(void)
{
	// Set up the server address to listen any address and the specified port
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(_port);

	// Create the socket
	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return std::cout << "[Listener::init_socket] Error creating socket with port" << _port << std::endl, 1;

	// Enable the socket to begin listening
	if (bind(_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		return std::cout << "[Listener::init_socket] Error binding socket with port" << _port << std::endl, 1;
	if (listen(_sockfd, BACKLOG) < 0)
		std::cout << "[Listener::init_socket] Error listening socket with port" << _port << std::endl, 1;

	// Set the socket to non-blocking so accept will not block
	if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) < 0)
		return std::cout << "[Listener::init_socket] Error fcntl" << std::endl, 1;

	return 0;
}