#pragma once
#include <iostream>
#include "Client.hpp"
#include <vector>
#include <poll.h>
#include <sstream>


class Server;

class CGI
{
	public:
	// Constructors and destructor

		CGI();
		CGI(const CGI & src);
		~CGI();

	// Setters and getters
		
	// Member functions
		
		void newCgi(Client &client, const std::string &filename);
		void closeCgi(Client &client);
		void loop(const Server &server);

	// Operator overloads

		CGI & operator=(const CGI & rhs);

	protected:
	private:
		struct ClientInfo
		{
			public:
				ClientInfo(const Client &client, int pid, int fdOut, int fdIn);
				bool operator==(const ClientInfo &rhs) const;
				std::istringstream _inputBuffer;
				std::ostringstream _outputBuffer;
				Client* _client;
				int _pid;
				int _fdOut;
				int _fdIn;
			private:
				ClientInfo();
		};
	// Atributes
		std::vector<pollfd> _pollfds;
		std::vector<ClientInfo> _clients;

	// Private member functions
		char **getEnv(const HttpRequest & req);

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const CGI &obj);
};

std::ostream &operator<<(std::ostream &os, const CGI &obj);
