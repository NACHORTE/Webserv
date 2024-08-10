#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <poll.h>

class FdHandler
{
	public:
	// Constructors and destructor


	// Setters and getters
		
		static void setTimeout(int timeout);
		static int getTimeout();

		static void addFd(int fd, int events);
		static void removeFd(int fd, bool close = true);

		static int getRevents(int fd);

	// Member functions

		static void poll();
		static void clear(bool close = true);

	// Operator overloads


	protected:
	private:
	// Atributes

		static int _timeout;
		static std::vector<struct pollfd> _fds;
		static std::map<int, int> _revents;
	
	// Private member functions

		FdHandler();
		FdHandler(const FdHandler & src);
		~FdHandler();
		FdHandler & operator=(const FdHandler & rhs);

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const FdHandler &obj);
};

std::ostream &operator<<(std::ostream &os, const FdHandler &obj);
