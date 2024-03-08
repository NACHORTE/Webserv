#pragma once
#include <iostream>
#include <set>
#include <list>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Client
{
	public:
	// Constructors and destructor
		Client(void);
		Client(const Client & src);
		~Client();

	// Setters and getters
		
		const std::string & getHost(void) const;
		const std::string & getResponse();
		// Returns part of the response and removes it from the client
		const std::string popResponse(size_t length = -1) const;

	// Member functions

		void addData(const std::string & data, size_t length);
		bool requestReady(void) const;
		bool responseReady(void) const;
		bool isError(void) const;
		void popRequest(void);
		bool timeout(void) const;
		bool keepAlive(void) const;

	// Operator overloads
		Client & operator=(const Client & rhs);

	protected:
	private:
	// Member attributes

		// Buffer for the http request being read from socket
		std::string requestBuffer;
		// Buffer for the http response being written to socket
		std::string responseBuffer;
		std::string host;
		size_t contentLength;

		std::set<std::string> _flags;
		bool requestReady;
		bool responseReady;
		bool isHeaderComplete;
		bool chunked;
		bool Error;
		bool keepAlive;
		bool timeout;

		std::list<std::pair<HttpRequest, HttpResponse> > _requests;
	// Private member functions

		void setHost(void);
		
	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Client &obj);
};

std::ostream &operator<<(std::ostream &os, const Client &obj);
