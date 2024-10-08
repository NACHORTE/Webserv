#pragma once
#include <iostream>
#include "Client.hpp"
#include <vector>
#include <set>
#include <sstream>
#include "defines.h"

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
		
		void newCgi(Client &client, const std::string &filename, Server &server);
		void closeCgi(Client &client);
        void closeCgi(size_t index);
		void loop(Server &server);

	// Operator overloads

		CGI & operator=(const CGI & rhs);

	protected:
	private:
		struct CgiClient
		{
            CgiClient();
            CgiClient(Client &client, int pid = -1, int fdOut = -1, int fdIn = -1);
            CgiClient(const CgiClient &src);
            ~CgiClient();
            CgiClient &operator=(const CgiClient &rhs);
            int write(size_t buff_size);
            int read(size_t buff_size);
            bool outBufferEmpty() const;
            std::string _inputBuffer;
            std::string _outputBuffer;
            size_t _outOffset;
            Client* _client;
            int _pid;
            int _fdOut; // fdOut = pipe to write to fork
            int _fdIn; // fdIn = pipe to read from fork
            bool _isDone;
			bool _somethingToRead;
		};
	// Atributes

        // Clients waiting for a CGI program to finish
		std::vector<CgiClient> _clients;

	// Private member functions
		
		void generateResponse(CgiClient &client, Server &server);
	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const CGI &obj);
};

std::ostream &operator<<(std::ostream &os, const CGI &obj);
