#pragma once
#include <iostream>

class Client
{
	public:
	// Constructors and destructor
		Client(void);
		Client(const Client & src);
		~Client();

	// Setters and getters
		
	// Member functions

	// Operator overloads
		Client & operator=(const Client & rhs);
	protected:
	private:

	friend std::ostream &operator<<(std::ostream &os, const Client &obj);
};

std::ostream &operator<<(std::ostream &os, const Client &obj);
