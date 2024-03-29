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
	server->sv_name = "";
	server->host = "";
	server->root = "";
	server->error_page = "";
	server->index = "";
	server->max_body = 0;
}

void init_location(t_location *location)
{
	location->path = "";
	location->root = "";
	location->index = "";
	location->autoindex = "";
	location->methods = "";
}

int check_duplicated(std::string var)
{
	if (var == "" || var == "0")
		return 0;
	return 1;
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

void print_location(t_location location)
{
	std::cout << "path: " << location.path << std::endl;
	std::cout << "root: " << location.root << std::endl;
	std::cout << "index: " << location.index << std::endl;
	std::cout << "autoindex: " << location.autoindex << std::endl;
	std::cout << "methods: " << location.methods << std::endl;
}

void print_server(t_server server)
{
	std::cout << "port: " << server.port << std::endl;
	std::cout << "server_name: " << server.sv_name << std::endl;
	std::cout << "host: " << server.host << std::endl;
	std::cout << "root: " << server.root << std::endl;
	std::cout << "error_page: " << server.error_page << std::endl;
	std::cout << "index: " << server.index << std::endl;
	std::cout << "max_body: " << server.max_body << std::endl;
	for (size_t i = 0; i < server.locations.size(); i++)
	{
		std::cout << "location " << i + 1 << ":" << std::endl;
		print_location(server.locations[i]);
	}
}

int read_location(t_location *location, std::istringstream &iss)
{
	std::string word;

	if (iss >> word && word[0] == '/')
	{
		location->path = word;
		//std::cout << "path: " << location->path << std::endl;
	}
	else
	{
		std::cerr << "Error reading config file, missing/error: path" << std::endl;
		return 0;
	}
	if (iss >> word && word != "{")
	{
		std::cerr << "Error reading config file (location), missing/error: {" << std::endl;
		return 0;
	}
	while (iss >> word)
	{
		if (word == "}")
		{
			return 1;
			break;
		}
		else if (word == "root")
		{
			if (check_duplicated(location->root))
			{
				std::cerr << "Error reading config file (location), duplicated: root" << std::endl;
				return 0;
			}
			if (iss >> word && back(word) == ';')
			{
				location->root = word;
				//std::cout << "root: " << location->root << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file (location), missing/error: root" << std::endl;
				return 0;
			}
		}
		else if (word == "index")
		{
			if (check_duplicated(location->index))
			{
				std::cerr << "Error reading config file (location), duplicated: index" << std::endl;
				return 0;
			}
			if (iss >> word && back(word) == ';')
			{
				location->index = word;
				//std::cout << "index: " << location->index << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file (location), missing/error: index" << std::endl;
				return 0;
			}
		}
		else if (word == "autoindex")
		{
			if (check_duplicated(location->autoindex))
			{
				std::cerr << "Error reading config file (location), duplicated: autoindex" << std::endl;
				return 0;
			}
			if (iss >> word && back(word) == ';')
			{
				location->autoindex = word;
				//std::cout << "autoindex: " << location->autoindex << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file (location), missing/error: autoindex" << std::endl;
				return 0;
			}
		}
		else if (word == "methods")
		{
			if (check_duplicated(location->methods))
			{
				std::cerr << "Error reading config file (location), duplicated: methods" << std::endl;
				return 0;
			}
			if (iss >> word && back(word) == ';')
			{
				location->methods = word;
				//std::cout << "methods: " << location->methods << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file (location), missing/error: methods" << std::endl;
				return 0;
			}
		}
		else
		{
			std::cerr << "Error reading config file (location), unknown: " << word << std::endl;
			return 0;
		}
	}
	return 0;
}

int init_socket(t_server *server)
{
	server->servaddr.sin_family = AF_INET;
	server->servaddr.sin_addr.s_addr = inet_addr(server->host.c_str());
	server->servaddr.sin_port = htons(server->port);

	if ((server->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Error creating socket" << std::endl;
		return 1;
	}
	if (bind(server->sockfd, (struct sockaddr *)&server->servaddr, sizeof(server->servaddr)) < 0)
	{
		std::cerr << "Error binding socket" << std::endl;
		return 1;
	}
	if (listen(server->sockfd, BACKLOG) < 0)
	{
		std::cerr << "Error listening socket" << std::endl;
		return 1;
	}
	return 0;
}

std::vector<t_server> read_config(const std::string& config_file)
{
	std::string input;
	std::string word;
	std::string content;
	int in_server = 0;
	input = readFile(config_file); //NOTE try catch
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
			if (init_socket(&servers[n_server]))
				return std::vector<t_server>();
			n_server++;
			continue;
		}
		else if (in_server && word == "location")
		{
			t_location location;
			init_location(&location);
			if (!read_location(&location, iss))
				return std::vector<t_server>();
			servers[n_server].locations.push_back(location);
			continue;
		}
		else if (in_server && word == "listen")
		{
			if (check_duplicated(int_to_string(servers[n_server].port)))
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), duplicated: listen" << std::endl;
				return std::vector<t_server>();
			}
			if (iss >> word && word.find_first_not_of("0123456789;") == std::string::npos  && back(word) == ';')
			{
				std::istringstream iss_num(word);
				iss_num >> servers[n_server].port;
				//std::cout << "port: " << servers[n_server].port << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), missing/error: listen" << std::endl;
				return std::vector<t_server>();
			}
		}
		else if (in_server && word == "server_name")
		{
			if (check_duplicated(servers[n_server].sv_name))
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), duplicated: server_name" << std::endl;
				return std::vector<t_server>();
			}
			if (iss >> word && back(word) == ';')
			{
				pop_back(word);
				servers[n_server].sv_name = word;
				//std::cout << "server_name: " << servers[n_server].sv_name << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), missing/error: server_name" << std::endl;
				return std::vector<t_server>();
			}
		}
		else if (in_server && word == "root")
		{
			if (check_duplicated(servers[n_server].root))
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), duplicated: root" << std::endl;
				return std::vector<t_server>();
			}
			if (iss >> word && back(word) == ';')
			{
				pop_back(word);
				servers[n_server].root = word;
				//std::cout << "root: " << servers[n_server].root << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), missing/error: root" << std::endl;
				return std::vector<t_server>();
			}
		}
		else if (in_server && word == "host")
		{
			if (check_duplicated(servers[n_server].host))
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), duplicated: host" << std::endl;
				return std::vector<t_server>();
			}
			if (iss >> word && back(word) == ';')
			{
				pop_back(word);
				servers[n_server].host = word;
				//std::cout << "host: " << servers[n_server].host << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), missing/error: host" << std::endl;
				return std::vector<t_server>();
			}
		}
		else if (in_server && word == "error_page")
		{
			if (check_duplicated(servers[n_server].error_page))
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), duplicated: error_page" << std::endl;
				return std::vector<t_server>();
			}
			if (iss >> word && back(word) == ';')
			{
				pop_back(word);
				servers[n_server].error_page = word;
				//std::cout << "error_page: " << servers[n_server].error_page << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), missing/error: error_page" << std::endl;
				return std::vector<t_server>();
			}
		}
		else if(in_server && word == "index")
		{
			if (check_duplicated(servers[n_server].index))
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), duplicated: index" << std::endl;
				return std::vector<t_server>();
			}
			if (iss >> word && back(word) == ';')
			{
				pop_back(word);
				servers[n_server].index = word;
				//std::cout << "index: " << servers[n_server].index << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), missing/error: index" << std::endl;
				return std::vector<t_server>();
			}
		}
		else if (in_server && word == "max_body")
		{
			if (check_duplicated(int_to_string(servers[n_server].max_body)))
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), duplicated: max_body" << std::endl;
				return std::vector<t_server>();
			}
			if (iss >> word && word.find_first_not_of("0123456789;") == std::string::npos && back(word) == ';')
			{
				std::istringstream iss_num(word);
				iss_num >> servers[n_server].max_body;
				//std::cout << "max_body: " << servers[n_server].max_body << std::endl;
			}
			else
			{
				std::cerr << "Error reading config file (server " << n_server + 1 << "), missing/error: max_body" << std::endl;
				return std::vector<t_server>();
			}
		}
		else
		{
			std::cerr << "Error reading config file, unknown: " << word << std::endl;
			return std::vector<t_server>();
		}
	}
	if (in_server == 1)
	{
		std::cerr << "Error reading config file, missing: }" << std::endl;
		return std::vector<t_server>();
	}
	for (size_t i = 0; i < servers.size(); i++)
	{
		std::cout << "server " << i + 1 << ":" << std::endl;
		print_server(servers[i]);
	}
	return servers;
}