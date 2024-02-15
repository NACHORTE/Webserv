#pragma once
#include <iostream>
#include <vector>
#include "Server.hpp"
#include "poll.h"
#include "Client.hpp"

class Listener
{
	public:
	// Constructors and destructor
		Listener(int port);
		~Listener();

	// Setters and getters
		void addServer(const Server & server);
		int getPort(void) const;
		
	// Member functions
		void loop(void);
		void acceptConnection(void);
		void readData(int fd);
		void sendData(int fd);
		void closeConnection(int fd);
		void closeListener(void);

	// Operator overloads
		Listener & operator=(const Listener & rhs);
	protected:
	private:
		int _port;
		int _sockfd;
		std::vector<Server> _servers;
		std::vector<std::pair<struct pollfd, Client> > _clients;

		int init_socket(void);

	friend std::ostream &operator<<(std::ostream &os, const Listener &obj);
};

std::ostream &operator<<(std::ostream &os, const Listener &obj);
