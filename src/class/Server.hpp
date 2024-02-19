#pragma once
#include <iostream>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Locations.hpp"
#include <map>
#include "set_config.hpp"

class Server
{
	public:
	// Constructors and destructor
		Server();
		Server(const Server & src);
		~Server();

	// Setters and getters
		void setPort(int port);
		void setClientMaxBodySize(int clientMaxBodySize);
		void setIndex(const std::string & index);
		void setRoot(const std::string & root);

		void addServerName(const std::string & serverName);
		void addLocation(const t_location & location);
		void addErrorPage(const std::string & errorPage);

		const int & getPort() const;
		const int & getClientMaxBodySize() const;
		const std::string & getIndex() const;
		const std::string & getRoot() const;
		const std::vector<std::string> & getServerNames() const;
		const std::vector<t_location> & getLocations() const;
		const std::vector<std::string> & getErrorPages() const;

	// Member functions

	// Operator overloads
		Server & operator=(const Server & rhs);

	protected:
	private:
	// Member attributes
		// Port number that the server will listen to
		int _port;
		// Maximum body size that the server will accept
    	int _clientMaxBodySize;
		// Default file to serve when the request is for a directory
    	std::string _index;
		// Root directory for the server
    	std::string _root;
		// List of server names that the server will respond to (host header in the request must match one of these names)
    	std::vector<std::string> _serverNames;
		// List of locations for the server
		std::vector<t_location> _locations;
		// List of error pages for the server
		std::vector<std::string> _errorPages;

	// Private member functions

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Server &obj);
};

std::ostream &operator<<(std::ostream &os, const Server &obj);

std::vector<Server> read_config(const std::string& config_file); //NOTE: MOVER A LA VERGA