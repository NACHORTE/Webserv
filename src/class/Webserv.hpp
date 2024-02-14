#pragma once
#include <iostream>
#include <vector>
#include "Listener.hpp"

class Webserv
{
	public:
	// Constructors and destructor
		Webserv(void);
		Webserv(const Webserv & src);
		~Webserv();

	// Setters and getters
		
	// Member functions
		void init(const std::string &configFile);
		void loop(void);

	// Operator overloads
		Webserv & operator=(const Webserv & rhs);
	protected:
	private:
	std::vector<Listener> _listeners;

	friend std::ostream &operator<<(std::ostream &os, const Webserv &obj);
};

std::ostream &operator<<(std::ostream &os, const Webserv &obj);
