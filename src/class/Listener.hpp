#pragma once
#include <iostream>
#include "Server.hpp"
#include "Client.hpp"
#include <vector>
#include <map>

class Listener
{
	public:
	// Constructors and destructor

		Listener(int port);
		~Listener();

	// Setters and getters

		void addServer(const Server & server);

		Server & getServer(const std::string & hostname) const;
		int getPort(void) const;
		
	// Member functions

		void loop(void);
		void closeListener(void);

	// Operator overloads

	protected:
	private:
	// Member attributes

		// Port number that the listener listens to
		int _port;
		// File descriptor for the listener (also in _cliens[0]->first.fd)
		int _sockfd;
		// Vector of all the servers (_serverMap points to these servers)
		std::vector<Server> _serverVector;
		// Map of all the servers (key is the hostname, multiple hostanames can point to the same server)
		std::map<std::string,Server *> _serverMap;
		// Vector of clients (can't be list because poll needs random access)
		std::vector<std::pair<struct pollfd, Client> > _clients;

	// Private member functions

		void readData(int fd, Client &client);
		void sendData(int fd, Client &client);
		void acceptConnection(void);
		void closeConnection(int clientIndex);

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Listener &obj);
};

std::ostream &operator<<(std::ostream &os, const Listener &obj);
