#include <iostream>
#include <fstream>
#include <sstream>
#include "set_config.hpp"

std::string read_data(std::string line, std::string key)
{
	std::size_t found = line.find(key);
	if (found != std::string::npos)
	{
		std::string port_str = line.substr(found + key.length() + 1);
		return port_str;
	}
	else
	{
		std::cerr << "Error reading config file, missing: " << key << std::endl;
		return "";
	}
}

void init_server(t_server *server)
{
	server->port = 0;
	server->sv_name = "Default";
	server->host = "";
	server->root = "";
	server->error_page = "";
	server->index = "";
	server->max_body = 0;
}

int check_full_server(t_server *server, int n_server)
{
	if (server->port == 0)
	{
		std::cerr << "Error reading config file (server " << n_server + 1 << "), missing: port" << std::endl;
		return 0;
	}
	if (server->sv_name == "")
	{
		std::cerr << "Error reading config file (server " << n_server + 1 << "), missing: server_name" << std::endl;
		return 0;
	}
	if (server->host == "")
	{
		std::cerr << "Error reading config file (server " << n_server + 1 << "), missing: host" << std::endl;
		return 0;
	}
	if (server->root == "")
	{
		std::cerr << "Error reading config file (server " << n_server + 1 << "), missing: root" << std::endl;
		return 0;
	}
	if (server->error_page == "")
	{
		std::cerr << "Error reading config file (server " << n_server + 1 << "), missing: error_page" << std::endl;
		return 0;
	}
	if (server->index == "")
	{
		std::cerr << "Error reading config file (server " << n_server + 1 << "), missing: index" << std::endl;
		return 0;
	}
	if (server->max_body == 0)
	{
		std::cerr << "Error reading config file (server " << n_server + 1 << "), missing: max_body" << std::endl;
		return 0;
	}
	return 1;
}

std::vector<t_server> read_config(const std::string& config_file)
{
	std::string input;
	std::string word;
	std::string content;
	int in_server = 0;
	input = readFile(config_file);
	std::istringstream iss(input);
	std::vector <t_server> servers;
	int n_server = 0;

	std::cout << input << std::endl;
	while (iss >> word)
	{
		if (!in_server && word == "server")
        {
			if (iss >> word && word == "{")
			{
				in_server = 1;
				t_server server;
				init_server(&server);
				servers.push_back(server);
			}
			else
				return std::vector<t_server>();
            continue;
        }
		else if (in_server && word == "}")
		{
			in_server = 0;
			if (!check_full_server(&servers[n_server], n_server))
				return std::vector<t_server>();
			n_server++;
			continue;
		}
		else if (in_server && word == "listen")
		{
			if (iss >> word && word.find_first_not_of("0123456789;") == std::string::npos  && word.back() == ';')
			{
				std::istringstream iss_num(word);
				iss_num >> servers[n_server].port;
				//std::cout << "port: " << servers[n_server].port << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file, missing/error: listen" << std::endl;
				return std::vector<t_server>();
			}
		}
		else if (in_server && word == "server_name")
		{
			if (iss >> word && word.back() == ';')
			{
				word.pop_back();
				servers[n_server].sv_name = word;
				//std::cout << "server_name: " << servers[n_server].sv_name << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file, missing/error: server_name" << std::endl;
				return std::vector<t_server>();
			}
		}
		else if (in_server && word == "root")
		{
			if (iss >> word && word.back() == ';')
			{
				servers[n_server].root = word;
				//std::cout << "root: " << servers[n_server].root << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file, missing/error: root" << std::endl;
				return std::vector<t_server>();
			}
		}
		else if (in_server && word == "host")
		{
			if (iss >> word && word.back() == ';')
			{
				servers[n_server].host = word;
				//std::cout << "host: " << servers[n_server].host << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file, missing/error: host" << std::endl;
				return std::vector<t_server>();
			}
		}
		else if (in_server && word == "error_page")
		{
			if (iss >> word && word.back() == ';')
			{
				servers[n_server].error_page = word;
				//std::cout << "error_page: " << servers[n_server].error_page << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file, missing/error: error_page" << std::endl;
				return std::vector<t_server>();
			}
		}
		else if(in_server && word == "index")
		{
			if (iss >> word && word.back() == ';')
			{
				servers[n_server].index = word;
				//std::cout << "index: " << servers[n_server].index << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file, missing/error: index" << std::endl;
				return std::vector<t_server>();
			}
		}
		else if (in_server && word == "max_body")
		{
			if (iss >> word && word.find_first_not_of("0123456789;") == std::string::npos && word.back() == ';')
			{
				std::istringstream iss_num(word);
				iss_num >> servers[n_server].max_body;
				//std::cout << "max_body: " << servers[n_server].max_body << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file, missing/error: max_body" << std::endl;
				return std::vector<t_server>();
			}
		}
		else
		{
			std::cerr << "Error reading config file, unknown: " << word << std::endl;
			return std::vector<t_server>();
		}
	}
	return servers;
}