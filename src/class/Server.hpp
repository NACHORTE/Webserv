#pragma once
#include <iostream>
#include <map>
#include "Client.hpp"
#include <set>
#include "LocationContainer.hpp"
#include "CGI.hpp"

class CGI;

class Server
{
	public:
	// Constructors and destructor

		Server();
		Server(const Server & src);
		~Server();

	// Setters and getters

		void setPort(int port);
		int getPort() const;

		void setMaxBodySize(size_t clientMaxBodySize);
		size_t getMaxBodySize() const;

		void setIndex(const std::string & index);
		const std::string & getIndex() const;

		void setRoot(const std::string & root);
		const std::string & getRoot() const;
	
		void addServerName(const std::string & serverName);
		const std::set<std::string> & getServerNames() const;

		void setErrorPages(const std::map<int, std::set<std::string> > & errorPages);
		const std::map<int, std::set<std::string> > & getErrorPages() const;
		void addErrorPage(int error_code, const std::string & errorPage);
		void addErrorPage(const std::string & errorPage);

		void addClient(Client &client);
		void removeClient(Client &client);

		void setLocationContainer(const std::vector<Location> & LocationContainer);
		const LocationContainer & getLocationContainer() const;
		bool addLocation(Location location);

	// Member functions

		HttpResponse errorResponse(int error_code, const std::string & phrase = "", const std::string & msg= "") const;
		void loop();

	// Operator overloads

		Server & operator=(const Server & rhs);

	protected:
	private:
	// Member attributes

		// Port number that the server will listen to
		int _port;
		// Maximum body size that the server will accept
		size_t _maxBodySize;
		// Default file to serve when the request is for a directory
    	std::string _index;
		// Root directory for the server
    	std::string _root;
		// List of server names that the server will respond to (host header in the request must match one of these names)
    	std::set<std::string> _serverNames;
		// List of error pages for the server
		std::map<int, std::set<std::string> > _errorPages;
		// List of clients the server is generating a response for
		std::set<Client *> _clients;
		// List of locations that the server will serve
		LocationContainer _locations;
		// Map of the methods the server can handle (GET, POST, DELETE)
		std::map<std::string, void (Server::*)(Client &, const Location &)> _methodsMap;
		// _activeCGI holds the list of clients that are waiting for a CGI program to finish
		CGI _activeCGI;

	// Private member functions

		void GET(Client & client, const Location & loc);
		void POST(Client & client, const Location & loc);
		void DELETE(Client & client, const Location & loc);

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Server &obj);
};

std::ostream &operator<<(std::ostream &os, const Server &obj);
