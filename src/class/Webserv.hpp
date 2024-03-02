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
		
	// Member functions

		void init(const std::string &configFile);
		void loop(void);
		void addServer(const Server &server);

	// Operator overloads

	protected:
	private:
	// Member attributes

		// Each listener listens to a different port and has its own list of servers
		std::vector<Listener> _listeners;

	// Private member functions

		std::vector<Server> read_config(const std::string &configFile);

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Webserv &obj);
};

std::ostream &operator<<(std::ostream &os, const Webserv &obj);
