#include <iostream>
#include <fstream>
#include <sstream>
#include "set_config.hpp"
#include "Server.hpp"

int check_duplicated(std::string var)
{
	if (var == "" || var == "0")
		return 0;
	return 1;
}

int check_full_server(Server *server, int n_server)
{
	if (server->getPort() == 0)
	{
		std::cout << "Error reading config file (server " << n_server + 1 << "), missing: port" << std::endl;
		return 0;
	}
	if (server->getServerName() == "")
	{
		std::cout << "Error reading config file (server " << n_server + 1 << "), missing: server_name" << std::endl;
		return 0;
	}
	if (server->getHost() == "")
	{
		std::cout << "Error reading config file (server " << n_server + 1 << "), missing: host" << std::endl;
		return 0;
	}
	if (server->getRoot() == "")
	{
		std::cout << "Error reading config file (server " << n_server + 1 << "), missing: root" << std::endl;
		return 0;
	}
	if (server->getErrorPage() == "")
	{
		std::cout << "Error reading config file (server " << n_server + 1 << "), missing: error_page" << std::endl;
		return 0;
	}
	if (server->getIndex() == "")
	{
		std::cout << "Error reading config file (server " << n_server + 1 << "), missing: index" << std::endl;
		return 0;
	}
	if (server->getMaxBody() == 0)
	{
		std::cout << "Error reading config file (server " << n_server + 1 << "), missing: max_body" << std::endl;
		return 0;
	}
	return 1;
}

int read_location(t_location *location, std::istringstream &iss)
{
	std::string word;

	if (iss >> word && word[0] == '/')
	{
		location->setPath(word);
	}
	else
	{
		std::cout << "Error reading config file, missing/error: path" << std::endl;
		return 0;
	}
	if (iss >> word && word != "{")
	{
		std::cout << "Error reading config file (location), missing/error: {" << std::endl;
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
			if (check_duplicated(location->getRoot()))
			{
				std::cout << "Error reading config file (location), duplicated: root" << std::endl;
				return 0;
			}
			if (iss >> word && back(word) == ';')
			{
				location->setRoot(word);
				//std::cout << "root: " << location->root << std::endl;
			}
			else
			{
				std::cout << "Error reading config file (location), missing/error: root" << std::endl;
				return 0;
			}
		}
		else if (word == "index")
		{
			if (check_duplicated(location->getIndex()))
			{
				std::cout << "Error reading config file (location), duplicated: index" << std::endl;
				return 0;
			}
			if (iss >> word && back(word) == ';')
			{
				location->setIndex(word);
			}
			else
			{
				std::cout << "Error reading config file (location), missing/error: index" << std::endl;
				return 0;
			}
		}
		else if (word == "autoindex")
		{
			if (check_duplicated(location->getAutoindex()))
			{
				std::cout << "Error reading config file (location), duplicated: autoindex" << std::endl;
				return 0;
			}
			if (iss >> word && back(word) == ';')
			{
				location->setAutoindex(word);
				//std::cout << "autoindex: " << location->autoindex << std::endl;
			}
			else
			{
				std::cout << "Error reading config file (location), missing/error: autoindex" << std::endl;
				return 0;
			}
		}
		else if (word == "methods")
		{
			if (check_duplicated(location->getMethods()))
			{
				std::cout << "Error reading config file (location), duplicated: methods" << std::endl;
				return 0;
			}
			if (iss >> word && back(word) == ';')
			{
				location->setMethods(word);
				//std::cout << "methods: " << location->methods << std::endl;
			}
			else
			{
				std::cout << "Error reading config file (location), missing/error: methods" << std::endl;
				return 0;
			}
		}
		else
		{
			std::cout << "Error reading config file (location), unknown: " << word << std::endl;
			return 0;
		}
	}
	return 0;
}

std::vector<Server> read_config(const std::string& config_file)
{
	std::string input;
	std::string word;
	std::string content;
	int in_server = 0;
	input = readFile(config_file); //NOTE try catch
	std::istringstream iss(input);
	std::vector <Server> servers;
	int n_server = 0;

	while (iss >> word)
	{
		if (!in_server && word == "server")
        {
			if (iss >> word && word == "{")
			{
				in_server = 1;
				Server server;
				servers.push_back(server);
			}
			else
				return std::vector<Server>();
            continue;
        }
		else if (in_server && word == "}")
		{
			in_server = 0;
			//if fails to check full server, return empty vector
			if (!check_full_server(&servers[n_server], n_server))
				return std::vector<Server>();
			n_server++;
			continue;
		}
		else if (in_server && word == "location")
		{
			t_location location;
			if (!read_location(&location, iss))
				return std::vector<Server>();
			servers[n_server].addLocation(location);
			continue;
		}
		else if (in_server && word == "listen")
		{
			if (check_duplicated(int_to_string(servers[n_server].getPort())))
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), duplicated: listen" << std::endl;
				return std::vector<Server>();
			}
			if (iss >> word && word.find_first_not_of("0123456789;") == std::string::npos  && back(word) == ';')
			{
				std::istringstream iss_num(word);
				int Portaux;
				iss_num >> Portaux;
				servers[n_server].setPort(Portaux);
			}
			else
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), missing/error: listen" << std::endl;
				return std::vector<Server>();
			}
		}
		else if (in_server && word == "server_name")
		{
			if (check_duplicated(servers[n_server].getServerName()))
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), duplicated: server_name" << std::endl;
				return std::vector<Server>();
			}
			if (iss >> word && back(word) == ';')
			{
				pop_back(word);
				servers[n_server].setServerName(word);
			}
			else
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), missing/error: server_name" << std::endl;
				return std::vector<Server>();
			}
		}
		else if (in_server && word == "root")
		{
			if (check_duplicated(servers[n_server].getRoot()))
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), duplicated: root" << std::endl;
				return std::vector<Server>();
			}
			if (iss >> word && back(word) == ';')
			{
				pop_back(word);
				servers[n_server].setRoot(word);
			}
			else
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), missing/error: root" << std::endl;
				return std::vector<Server>();
			}
		}
		else if (in_server && word == "host")
		{
			if (check_duplicated(servers[n_server].getHost()))
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), duplicated: host" << std::endl;
				return std::vector<Server>();
			}
			if (iss >> word && back(word) == ';')
			{
				pop_back(word);
				servers[n_server].setHost(word);
			}
			else
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), missing/error: host" << std::endl;
				return std::vector<Server>();
			}
		}
		else if (in_server && word == "error_page")
		{
			if (check_duplicated(servers[n_server].getErrorPage()))
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), duplicated: error_page" << std::endl;
				return std::vector<Server>();
			}
			if (iss >> word && back(word) == ';')
			{
				pop_back(word);
				servers[n_server].setErrorPage(word);
			}
			else
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), missing/error: error_page" << std::endl;
				return std::vector<Server>();
			}
		}
		else if(in_server && word == "index")
		{
			if (check_duplicated(servers[n_server].getIndex()))
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), duplicated: index" << std::endl;
				return std::vector<Server>();
			}
			if (iss >> word && back(word) == ';')
			{
				pop_back(word);
				servers[n_server].setIndex(word);
			}
			else
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), missing/error: index" << std::endl;
				return std::vector<Server>();
			}
		}
		else if (in_server && word == "max_body")
		{
			if (check_duplicated(int_to_string(servers[n_server].getMaxBody())))
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), duplicated: max_body" << std::endl;
				return std::vector<Server>();
			}
			if (iss >> word && word.find_first_not_of("0123456789;") == std::string::npos && back(word) == ';')
			{
				std::istringstream iss_num(word);
				int max_body;
				iss_num >> max_body;
				servers[n_server].setMaxBody(max_body);
			}
			else
			{
				std::cout << "Error reading config file (server " << n_server + 1 << "), missing/error: max_body" << std::endl;
				return std::vector<Server>();
			}
		}
		else
		{
			std::cout << "Error reading config file, unknown: " << word << std::endl;
			return std::vector<Server>();
		}
	}
	if (in_server == 1)
	{
		std::cout << "Error reading config file, missing: }" << std::endl;
		return std::vector<Server>();
	}
	return servers;
}