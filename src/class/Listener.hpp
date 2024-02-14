#pragma once
#include <iostream>
#include <vector>
#include "Server.hpp"

class Listener
{
	public:
	// Constructors and destructor
		Listener(int port);
		~Listener();

	// Setters and getters
		int getPort(void) const;
		void addServer(const Server & server);
		
	// Member functions

	// Operator overloads
		Listener & operator=(const Listener & rhs);
	protected:
	private:
	int _port;
	int _sockfd;
	std::vector<Server> _servers;
	int init_socket(void);

	friend std::ostream &operator<<(std::ostream &os, const Listener &obj);
};

std::ostream &operator<<(std::ostream &os, const Listener &obj);
