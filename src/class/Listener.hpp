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
		void loop();

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
		// Map of all the servers (key is the hostname, multiple hostnames can point to the same server)
		std::map<std::string,Server *> _serverMap;
		// Vector of clients
		std::list<Client> _clients;

	// Private member functions

		int acceptConnection();
		void readData(Client &client);
		int sendData(Client &client);
		void closeConnection(Client &client);

		void sendToServer(Client &client);
		void errorResponse(Client & client, int errorCode, const std::string & phrase = "", const std::string & msg= "");

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Listener &obj);
};

std::ostream &operator<<(std::ostream &os, const Listener &obj);
