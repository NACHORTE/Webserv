#pragma once
#include <Server.hpp>
#include <list>

// Main function to read the config file
std::vector<Server> readConfig(const std::string& configFile);
// Auxiliary functions to read the different blocks
// server block
void readServer(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv);
void readListen(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv);
void readServerName(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv);
void readRoot(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv);
void readIndex(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv);
void readErrorPage(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv);
void readMaxBody(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv);

// location block
void readLocation(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv);
void readAlias(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv);
void readAutoIndex(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv);
void readCGI(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Server & serv);