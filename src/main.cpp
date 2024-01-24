#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "Server.hpp"
#include <sstream>
#include <cstring>
#include <fstream>
#include <vector>
#include <poll.h>
#include <fcntl.h>
#include "defines.h"
#include "colors.h"
#include "utils.hpp"
#include "set_config.hpp"

unsigned long millis()
{
	static bool first = 1;
	static unsigned long first_time;
	struct timeval tv;
	if (gettimeofday(&tv,NULL)==-1)
		throw (std::runtime_error("gettimeofday"));
	if (first)
	{
		first = 0;
		first_time = tv.tv_sec *1000 + tv.tv_usec / 1000;
	}
	unsigned long this_time = tv.tv_sec *1000 + tv.tv_usec / 1000;
	return this_time - first_time;
}

static void print_long_str(const std::string & str, size_t max_size = 1000)
{
	if (str.size() > max_size)
		std::cout << str.substr(0, max_size) << "...";
	else
		std::cout << str;
}

int main(int argc, char **argv)
{
	struct sockaddr_in client;
	HttpResponse response;
	Server server;

	if (argc != 2)
	{
		std::cout << "[SERVER] Usage: ./Webserv <config_file>\n";
		return 1;
	}
	if (ok_config(argv[1]))
		return 1;

	std::vector<t_server> servers = read_config(argv[1]);
	if (servers.size() == 0)
		return 1;

	for (size_t i = 0; i < servers.size(); i++)
	{
		if (fcntl(servers[i].sockfd, F_SETFL, O_NONBLOCK) < 0)
		{
			std::cout << "[SERVER] Error fcntl\n";
			return 1;
		}
	}

	while(1)
	{
		unsigned int len = sizeof(client);
		std::vector<int> connfds(servers.size(), 0);
		for (size_t i = 0; i < servers.size(); i++)
		{
			connfds[i] = accept(servers[i].sockfd, (struct sockaddr *)&client, &len); //NOTE blocking call
			if (connfds[i] < 0)
			{
				//std::cout << "[SERVER] No connections\n";
				continue;
			}
			std::cout << "new_client!\n";
		}
		int some_connection = 0;
		for (size_t i = 0; i < servers.size(); i++)
		{
			if (connfds[i] > 0)
			{
				some_connection = 1;
				break;
			}
		}
		if (!some_connection)
			continue;
		std::vector<struct pollfd> fds(servers.size());
		for (size_t i = 0; i < servers.size(); i++)
		{
			fds[i].fd = connfds[i];
			fds[i].events = POLLIN;
		}
		char buff[BUFF_SIZE];
		int n;
		std::string msg;
		size_t fds_size = fds.size();
		std::cout << "polling\n";
		std::cout << "fds_size: " << fds_size << "\n";
		while ((n = poll(&fds[0],fds_size, 1000)) > 0)
		{
			std::cout << "inside polling\n";
			if (n < 0)
			{
			std::cout << "[SERVER] Error polling\n";
			return 1;
			}
			int some_event = 0;
			for (size_t i = 0; i < fds_size; i++)
			{
				std::cout << "inside for " << i << "\n";
				if (fds[i].revents & POLLIN)
				{
					some_event = 1;
					std::cout << "inside if\n";
					int n_read = 0;
					if (fcntl(fds[i].fd, F_SETFL, O_NONBLOCK) < 0)
					{
						std::cout << "[SERVER] Error fcntl\n";
						return 1;
					}
					while (poll(&fds[0],fds_size, 1) > 0 && (fds[i].revents & POLLIN) && ((n_read = read(fds[i].fd, buff, BUFF_SIZE)) > 0))  //NOTE do while for little buff
					{
						std::cout << "n_read: " << (int)n_read << "\n";
						msg += std::string(buff,n_read);
					}
					std::cout << "fuera while\n";

					HttpRequest request(msg);
					std::string request_str = request();
					std::cout << "-\nRequest:\n" << BLUE;
					print_long_str(request_str, 5000);
					std::cout << RESET << "-" << std::endl;

					response.generate(request, server._allowed_paths, server._allowed_methods);
					std::string response_str = response();
					std::cout << "-\nResponse:\n" << GREEN;
					print_long_str(response_str, 5000);
					std::cout << RESET << "-" << std::endl;

					write(fds[i].fd, response_str.c_str(), response_str.size());

					memset(buff, 0, BUFF_SIZE);
					msg.clear();
					close(connfds[i]);
				}
				std::cout << "end for\n";
			}
			if (!some_event)
				break;
		}
		for (size_t i = 0; i < fds_size; i++)
			{
				std::cout << "fds[" << i << "].fd: " << fds[i].fd << "\n";
				std::cout << "fds[" << i << "].events: " << fds[i].events << "\n";
				std::cout << "fds[" << i << "].revents: " << fds[i].revents << "\n";
				close(fds[i].fd);
			}
		std::cout << "FIN POLL\n";
	}
	return 0;
}
