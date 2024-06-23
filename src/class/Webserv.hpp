#pragma once
#include <iostream>
#include <vector>
#include "Listener.hpp"

class Webserv
{
	public:
	// Constructors and destructor

		Webserv(void);
		~Webserv();

	// Setters and getters

		void addServer(const Server &server);
		
	// Member functions

		void init(const std::string &configFile);
		void loop();

	// Operator overloads

	protected:
	private:
	// Member attributes

		// Each listener listens to a different port and has its own list of servers
		std::vector<Listener> _listeners;

	// Private member functions

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Webserv &obj);
};

std::ostream &operator<<(std::ostream &os, const Webserv &obj);
