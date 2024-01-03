#include "set_config.hpp"
#include "utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

config::config(std::string config_file)
{
	read_config(config_file);
}

config::~config()
{
}

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

int config::read_config(const std::string& config_file)
{
	std::string input;
	std::string word;
	std::string content;
	int in_server = 0;
	input = readFile(config_file);
	std::istringstream iss(input);

	std::cout << input << std::endl;
	while (iss >> word)
	{
		if (!in_server && word == "server")
        {
			if (iss >> word && word == "{")
				in_server = 1;
			else
				return 1;
            continue;
        }
		else if (in_server && word == "}")
		{
			in_server = 0;
			continue;
		}
		else if (in_server && word == "listen")
		{
			if (iss >> word && word.find_first_not_of("0123456789;") == std::string::npos)
			{
				std::istringstream iss_num(word);
				iss_num >> port;
				std::cout << "port: " << port << std::endl;
			}
			else
				return 1;
		}
		else if (in_server && word == "server_name")
		{
			if (iss >> word)
			{
				sv_name = word;
				std::cout << "server_name: " << sv_name << std::endl;
			}
			else
				return 1;
		}
		else if (in_server && word == "root")
		{
			if (iss >> word)
			{
				root = word;
				std::cout << "root: " << root << std::endl;
			}
			else
				return 1;
		}
		else if (in_server && word == "host")
		{
			if (iss >> word)
			{
				host = word;
				std::cout << "host: " << host << std::endl;
			}
			else
				return 1;
		}
		else if (in_server && word == "error_page")
		{
			if (iss >> word)
			{
				error_page = word;
				std::cout << "error_page: " << error_page << std::endl;
			}
			else
				return 1;
		}
		else if(in_server && word == "index")
		{
			if (iss >> word)
			{
				index = word;
				std::cout << "index: " << index << std::endl;
			}
			else
				return 1;
		}
		else if (in_server && word == "max_body")
		{
			if (iss >> word && word.find_first_not_of("0123456789;") == std::string::npos)
			{
				std::istringstream iss_num(word);
				iss_num >> max_body;
				std::cout << "max_body: " << max_body << std::endl;
			}
			else
				return 1;
		}
		else
		{
			std::cerr << "Error reading config file, unknown: " << word << std::endl;
			return 1;
		}
	}
	return 0;
}