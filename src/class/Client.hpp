#pragma once
#include <iostream>
#include <set>
#include <list>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <ctime>

class Client
{
	public:
	// Constructors and destructor
		Client(void);
		Client(const Client & src);
		~Client();

	// Setters and getters
		
		std::string getHost(void) const;
		std::string getResponse() const;

	// Member functions

		void addData(const std::string & data);
		bool requestReady(void) const;
		bool responseReady(void) const;
		bool timeout(void) const;
		bool keepAlive(void) const;
		// Returns part from the begining of the  response and removes it from the client
		const std::string popResponse(size_t length = -1);
		// Deletes the last request and response from _requests
		void popRequest(void);

	// Operator overloads
		Client & operator=(const Client & rhs);

	protected:
	private:
	// Member attributes

		bool _Error;
		clock_t _lastEventTime;
		// Queue of requests and responses (Most recent request is at the front of the list)
		std::list<std::pair<HttpRequest, HttpResponse> > _requests;
		std::set<std::string> _flags;
	// Private member functions

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Client &obj);
};

std::ostream &operator<<(std::ostream &os, const Client &obj);
