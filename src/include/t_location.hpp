#pragma once
#include <string>
#include "utils.hpp"
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include "defines.h"
#include <arpa/inet.h>

class t_location
{
	public:
	void setPath(const std::string & path);
	void setRoot(const std::string & root);
	void setIndex(const std::string & index);
	void setAutoindex(const std::string & autoindex);
	void setMethods(const std::string & methods);
	std::string getPath(void) const;
	std::string getRoot(void) const;
	std::string getIndex(void) const;
	std::string getAutoindex(void) const;
	std::string getMethods(void) const;
	private:
	std::string path;
	std::string root;
	std::string index;
	std::string autoindex;
	std::string methods;
};
