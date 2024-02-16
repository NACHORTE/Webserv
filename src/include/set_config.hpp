#pragma once
#include <string>
#include "utils.hpp"
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include "defines.h"
#include <arpa/inet.h>
#include <iostream>

class t_location
{
	public:
	std::string path;
	std::string root;
	std::string index;
	std::string autoindex;
	std::string methods;
};

std::ostream &operator<<(std::ostream &os, const t_location &obj)
{
	os << "path: " << obj.path << std::endl;
	os << "root: " << obj.root << std::endl;
	os << "index: " << obj.index << std::endl;
	os << "autoindex: " << obj.autoindex << std::endl;
	os << "methods: " << obj.methods << std::endl;
	return (os);
}
