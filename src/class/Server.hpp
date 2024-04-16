#pragma once
#include <iostream>
#include <map>
#include "Location.hpp"
#include "Client.hpp"
#include <list>
#include <set>

class Server
{
	public:
	// Constructors and destructor

		Server();
		Server(const Server & src);
		~Server();

	// Setters and getters

		void setPort(int port);
		void setClientMaxBodySize(size_t clientMaxBodySize);
		void setIndex(const std::string & index);
		void setRoot(const std::string & root);
		void setLocations(const std::vector<Location> & locations);
		void setErrorPages(const std::vector<std::string> & errorPages);

		void addServerName(const std::string & serverName);
		void addLocation(const Location & location);
		void addErrorPage(const std::string & errorPage);
		void addClient(Client &client);

		int getPort() const;
		int getClientMaxBodySize() const;
		const std::string & getIndex() const;
		const std::string & getRoot() const;
		const std::set<std::string> & getServerNames() const;
		const std::vector<Location> & getLocations() const;
		const std::vector<std::string> & getErrorPages() const;

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
		size_t _clientMaxBodySize;
		// Default file to serve when the request is for a directory
    	std::string _index;
		// Root directory for the server
    	std::string _root;
		// List of server names that the server will respond to (host header in the request must match one of these names)
    	std::set<std::string> _serverNames;
		// List of locations for the server
		std::vector<Location> _locations;
		// List of error pages for the server
		std::vector<std::string> _errorPages;
		// List of clients the server is generating a response for
		std::set<Client *> _clients;

		std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)> _allowed_methods;
		Locations _allowed_paths;
	// Private member functions

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Server &obj);
};

std::ostream &operator<<(std::ostream &os, const Server &obj);
