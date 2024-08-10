#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <poll.h>

class MyPoll
{
	public:
	// Constructors and destructor

		MyPoll(int timeout = 0);
		MyPoll(const MyPoll & src);
		~MyPoll();

	// Setters and getters
		
		void setTimeout(int timeout);
		int getTimeout() const;

		void addFd(int fd, int events);
		void removeFd(int fd, bool close = true);

		int getRevents(int fd) const;

	// Member functions

		void poll();
		void clear(bool close = true);

	// Operator overloads

		MyPoll & operator=(const MyPoll & rhs);

	protected:
	private:
	// Atributes

		int _timeout;
		std::vector<struct pollfd> _fds;
		std::map<int, int> _revents;
	
	// Private member functions

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const MyPoll &obj);
};

std::ostream &operator<<(std::ostream &os, const MyPoll &obj);
