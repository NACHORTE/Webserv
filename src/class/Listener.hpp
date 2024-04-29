#pragma once
#include <iostream>
#include "Server.hpp"
#include "Client.hpp"
#include <vector>
#include <map>
#include <poll.h>

class Listener
{
	public:
	// Constructors and destructor

		Listener(int port);
		Listener(const Listener &src);
		~Listener();

	// Setters and getters

		void addServer(const Server & server);

		Server & getServer(const std::string & hostname) const; // XXX UNUSED ?
		int getPort(void) const;
		
	// Member functions

		// Main loop of the listener (accepts connections, reads data, sends data)
		// Only runs once, to run it again, call it again
		void loop(void);
		void closeFds();

	// Operator overloads
		
		Listener &operator=(const Listener &src);

	protected:
	private:
	// Member attributes

		// Port number that the listener listens to
		int _port;
		// File descriptor for the listener (also in _cliens[0]->first.fd)
		int _sockfd;
		// Vector of all the servers (_serverMap points to these servers)
		std::vector<Server> _serverVector; //NOTE Server class has a pointer to clients that stop working when the client vector reallocates 
		// Map of all the servers (key is the hostname, multiple hostanames can point to the same server)
		std::map<std::string,Server *> _serverMap; //NOTE when _serverVector pushes new servers, it reallocates all servers so the pointers don't work anymore
		// Vector of clients
		std::vector<struct pollfd> _pollfds;
		std::vector<Client> _clients;

	// Private member functions

		int acceptConnection(void);
		int readData(int fd, Client &client);
		int sendData(int fd, Client &client);
		int closeConnection(int clientIndex);

		void sendToServer(Client &client);

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Listener &obj);
};

std::ostream &operator<<(std::ostream &os, const Listener &obj);
