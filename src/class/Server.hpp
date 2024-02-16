#pragma once
#include <iostream>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Locations.hpp"
#include <map>
#include "t_location.hpp"
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
		void setHost(const std::string & host);
		void setRoot(const std::string & root);
		void setErrorPage(const std::string & errorPage);
		void setIndex(const std::string & index);
		void setServerName(const std::string & serverName);
		void setMaxBody(int maxBody);
		void addLocation(const t_location & location);

		int getPort(void) const;
		const std::string& getServerName(void) const;
		const std::string& getHost(void) const;
		const std::string& getRoot(void) const;
		const std::string& getErrorPage(void) const;
		const std::string& getIndex(void) const;
		std::vector<t_location> getLocations(void) const;
		int getMaxBody(void) const;

	// Member functions

	// Operator overloads
		Server & operator=(const Server & rhs);
	protected:
	private:
		std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)> _allowed_methods;
		Locations _allowed_paths;
		std::vector<std::string> _errorPages;
		int port;
    	std::string sv_name;
    	std::string host;
    	std::string root;
    	std::string error_page;
    	std::string index;
		std::vector<t_location> locations;
    	int max_body;
		
	friend std::ostream &operator<<(std::ostream &os, const Server &obj);
};

std::ostream &operator<<(std::ostream &os, const Server &obj);

std::vector<Server> read_config(const std::string& config_file); //NOTE: MOVER A LA VERGA