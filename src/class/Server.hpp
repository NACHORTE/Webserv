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
		Server();
		Server(const Server & src);
		~Server();
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
	public:
		int getPort(void) const;
		void setPort(int port);
		std::string getServerName(void) const;
		void setServerName(const std::string & serverName);
		std::string getHost(void) const;
		void setHost(const std::string & host);
		std::string getRoot(void) const;
		void setRoot(const std::string & root);
		std::string getErrorPage(void) const;
		void setErrorPage(const std::string & errorPage);
		std::string getIndex(void) const;
		void setIndex(const std::string & index);
		std::vector<t_location> getLocations(void) const;
		int getMaxBody(void) const;
		void setMaxBody(int maxBody);
		void addLocation(const t_location & location);
		
	friend std::ostream &operator<<(std::ostream &os, const Server &obj);
};

std::ostream &operator<<(std::ostream &os, const Server &obj);

std::vector<Server> read_config(const std::string& config_file); //NOTE: MOVER A LA VERGA