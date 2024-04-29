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
		Client(std::string IP = "", int port = 0);
		Client(const Client & src);
		~Client();

	// Setters and getters
		void setResponse(const HttpResponse & response);

		std::string getIP() const;
		int getPort() const;
		std::string getHost(void) const;
		std::string getResponse() const;
		const HttpRequest & getRequest(void);
		size_t getRequestCount(void) const;

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
		std::string _IP; // NOTE info only
		int _port; // NOTE info only

		bool _Error;
		clock_t _lastEventTime;
		// Queue of requests and responses (Most recent request is at the front of the list)
		std::list<std::pair<HttpRequest, HttpResponse> > _requests;
	// Private member functions

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Client &obj);
};

std::ostream &operator<<(std::ostream &os, const Client &obj);
