#pragma once
#include <iostream>
#include <map>
#include "Client.hpp"
#include <list>
#include <set>
#include "LocationContainer.hpp"

class Server
{
	public:
	// Constructors and destructor

		Server();
		Server(const Server & src);
		~Server();

	// Setters and getters

		void setPort(int port);
		void setMaxBodySize(size_t clientMaxBodySize);
		void setIndex(const std::string & index);
		void setRoot(const std::string & root);
		void setLocationContainer(const std::vector<Location> & LocationContainer);
		void setErrorPages(const std::map<int, std::string> & errorPages);

		void addServerName(const std::string & serverName);
		void addLocation(const Location & location);
		void addErrorPage(const std::string & errorPage);
		void addClient(Client &client);

		int getPort() const;
		int getClientMaxBodySize() const;
		const std::string & getIndex() const;
		const std::string & getRoot() const;
		const std::set<std::string> & getServerNames() const;
		const LocationContainer & getLocationContainer() const;
		const std::map<int, std::string> & getErrorPages() const;

		void removeClient(Client &client);

	// Member functions

		void loop();

	// Operator overloads
		Server & operator=(const Server & rhs);

	protected:
	private:
	// Member attributes

		// Port number that the server will listen to
		int _port;
		// Maximum body size that the server will accept
		size_t _maxBodySize; // TODO No va aun
		// Default file to serve when the request is for a directory
    	std::string _index; //XXX FUERA DE AQUI (aunque debería funcionar)
		// Root directory for the server
    	std::string _root; // TODO No va aun
		// List of server names that the server will respond to (host header in the request must match one of these names)
    	std::set<std::string> _serverNames;
		// List of error pages for the server
		std::map<int, std::string> _errorPages; // TODO: Implement error pages
		// List of clients the server is generating a response for
		std::set<Client *> _clients;
		// List of locations that the server will serve
		LocationContainer _locations;
		// Map of the methods the server can handle (GET, POST, DELETE)
		std::map<std::string, HttpResponse (*)(const HttpRequest &, const LocationContainer &)> _methodsMap;

		// _cgiClients holds the list of clients that are waiting for a CGI program to finish
		struct ClientInfo //TODO esto a otra clase alomejor no se
		{
			ClientInfo();
			ClientInfo(const Client &client, int pid = -1, int fdIn = -1, int fdOut = -1);
			bool operator==(const ClientInfo &rhs) const;
			bool operator<(const ClientInfo &rhs) const;
   			Client* _client;
    		int _pid;
   			int _fdOut;
   			int _fdIn;
		};
		std::set<ClientInfo> _cgiClients;

	// Private member functions

		// check whether a request is for a CGI program
		bool isCgi(const HttpRequest &request);
		// start a CGI program (fork, execve, pipe, etc.)
		int startCgi(const Client &client);
		HttpResponse cgiResponse(const ClientInfo &clientInfo) const;

		char **getPath(const HttpRequest & req);
		char **getEnv(const HttpRequest & req);
		
	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Server &obj);
};

std::ostream &operator<<(std::ostream &os, const Server &obj);
