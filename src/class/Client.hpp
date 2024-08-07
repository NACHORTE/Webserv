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
		Client(std::string IP = "", int port = 0);
		Client(const Client & src);
		~Client();

	// Setters and getters
		void setResponse(const HttpResponse & response);
        void setResponse(const std::string & response);

		std::string getIP() const;
		int getPort() const;
		std::string getHost(void) const;
		HttpResponse & getResponse();
		const HttpResponse & getResponseConst() const;
		HttpRequest & getRequest(void);
		const HttpRequest & getRequestConst(void) const;
		size_t getRequestCount(void) const;

		void error(bool error);
		bool error(void) const;

	// Member functions

		void addData(const std::string & data);
		bool requestReady(void) const;
		bool responseReady(void) const;
		bool timeout(void) const;
		bool keepAlive(void) const;
		void popRequest(void);

	// Operator overloads
		Client & operator=(const Client & rhs);

	protected:
	private:
	// Member attributes
		std::string _IP;
		int _port;

		clock_t _lastEventTime;
		// Queue of requests and responses (Most recent request is at the front of the list)
		std::list<std::pair<HttpRequest, HttpResponse> > _requests;

		bool _error;
	// Private member functions

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Client &obj);
};

std::ostream &operator<<(std::ostream &os, const Client &obj);
