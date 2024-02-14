#include "Listener.hpp"

int Listener::init_socket(void)
{
	// Set up the server address to listen any address and the specified port
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(_port);

	// Create the socket
	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return std::cout << "Error creating socket" << std::endl, 1;

	// Enable the socket to begin listening
	if (bind(_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		return std::cout << "Error binding socket" << std::endl, 1;
	}
	if (listen(_sockfd, BACKLOG) < 0)
	{
		std::cerr << "Error listening socket" << std::endl;
		return 1;
	}
	return 0;
}

Listener::Listener(int port) : _port(port)
{
	if (init_socket())
		throw std::exception();
}

Listener::~Listener()
{
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

int Listener::getPort(void) const
{
	return (_port);
}

void Listener::addServer(const Server &server)
{
	_servers.push_back(server);
}