#pragma once
#include <iostream>
#include "Server.hpp"
#include "Client.hpp"
#include <vector>
#include <map>
#include <list>

class Listener
{
	public:
	// Constructors and destructor

		Listener(int port);
		Listener(const Listener &src);
		~Listener();

	// Setters and getters

		void addServer(const Server & server);

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
		// File descriptor for the listener (also in _clients[0]->first.fd)
		int _sockfd;
		// Vector of all the servers (_serverMap points to these servers)
		std::list<Server> _serverList;
		// Map of all the servers (key is the hostname, multiple hostanames can point to the same server)
		std::map<std::string,Server *> _serverMap;
		// Vector of clients
		std::vector<struct pollfd> _pollfds;
		std::list<Client> _clients;

	// Private member functions

		int acceptConnection(void);
		int readData(int fd, Client &client);
		int sendData(int fd, Client &client);
		int closeConnection(int clientIndex);

		void sendToServer(Client &client);
		HttpResponse errorResponse(Client & client, int errorCode, const std::string & phrase = "", const std::string & msg= "");

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Listener &obj);
};

std::ostream &operator<<(std::ostream &os, const Listener &obj);
