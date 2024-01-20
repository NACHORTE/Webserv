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

int main(int argc, char **argv)
{
	//struct sockaddr_in servaddr, client;
	struct sockaddr_in client;
	//int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 29\n\nHello from server!";
	HttpResponse response;

	if (argc != 2)
	{
		std::cout << "[SERVER] Usage: ./server <config_file>\n";
		return 1;
	}
	if (ok_config(argv[1]))
		return 1;

	std::vector<t_server> servers = read_config(argv[1]);
	if (servers.size() == 0)
		return 1;

	/*if (sockfd == -1)
	{
		std::cout << "[SERVER] Error creating socket\n";
		return 1;
	}
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(SERV_HOST_ADDR);
	servaddr.sin_port = htons(SERV_PORT);

	if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
	{
		perror("[SERVER] Error binding socket");
		return 1;
	}
	if ((listen(sockfd, BACKLOG)) != 0)
	{
		std::cout << "[SERVER] Error listen\n";
		return 1;
	}*/

	//INIT RESPONSES MANUALLY
	response.set_status(200);
	response.set_body("text/plain","Recieved!");

	HttpResponse imageResponse;
	imageResponse.set_status(200);
	imageResponse.set_body("image/jpeg",readImageFile("imgs/goku.jpg"));

	HttpResponse htmlResponse;
	htmlResponse.set_status(200);
	htmlResponse.set_body("text/html",readFile("html/index.html"));

	HttpResponse htmlPrin;
	htmlPrin.set_status(200);
	htmlPrin.set_body("text/html",readFile("html/principal.html"));

	HttpResponse htmlSec;
	htmlSec.set_status(200);
	htmlSec.set_body("text/html",readFile("html/secundaria.html"));

	HttpResponse htmlUpload;
	htmlUpload.set_status(200);
	htmlUpload.set_body("text/html",readFile("html/upload.html"));

	HttpResponse htmlError;
	htmlError.set_status(404);
	htmlError.set_body("text/html",readFile("html/error.html"));

	/*if (fcntl(servers[0].sockfd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cout << "[SERVER] Error fcntl\n";
		return 1;
	}*/

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
		//std::cout << "[" << millis() << "] SERVER WAITING\n";
		unsigned int len = sizeof(client);
		//std::cout << "sockfd:" << servers[0].sockfd << "\n";
		
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
		/*int connfd = accept(servers[0].sockfd, (struct sockaddr *)&client, &len); //NOTE blocking call
		if (connfd < 0)
		{
			//std::cout << "[SERVER] No connections\n";
			continue;
		}
		std::cout << "new_client!\n";*/

		std::vector<struct pollfd> fds(servers.size());
		/*for (size_t i = 0; i < servers.size(); i++)
		{
			fds[i].fd = servers[i].sockfd;
			fds[i].events = POLLIN;
		}*/
		for (size_t i = 0; i < servers.size(); i++)
		{
			fds[i].fd = connfds[i];
			fds[i].events = POLLIN;
		}
		/*fds[0].fd = connfd;
		fds[0].events = POLLIN;*/

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
			for (size_t i = 0; i < fds_size; i++)
			{
				std::cout << "inside for " << i << "\n";
				if (fds[i].revents & POLLIN)
				{
					std::cout << "inside if\n";
					int n_read = read(fds[i].fd, buff, BUFF_SIZE);
					if (n_read < 0)
					{
						std::cout << "[SERVER] Error reading from socket\n";
						return 1;
					}
					std::cout << "n_read: " << n_read << "\n";
					msg += std::string(buff,n_read);
					std::cout << RED "buff:"<< "\n";
					for (size_t i = 0; i < static_cast<size_t>(n_read); i++)
					{
						if (buff[i] == '\0')
							std::cout << "\\0";
						else if (buff[i] == '\n')
							std::cout << buff[i];
						else if (buff[i] == '\r')
							std::cout << buff[i];
						else if (isprint(buff[i]))
							std::cout << buff[i];
						else
							std::cout << "\\x" << std::hex << (int)buff[i];
					}
					std::cout << RESET << "\n";
					if (strncmp(msg.c_str(), "GET / HTTP/1.1", strlen("GET / HTTP/1.1")) == 0)
					{
						std::cout << "\n index \n";
						n = write(connfds[i], htmlResponse.to_string().c_str(), htmlResponse.to_string().length());
					}
					else if (strncmp(msg.c_str(), "GET /imgs/goku.jpg HTTP/1.1", strlen("GET /imgs/goku.jpg HTTP/1.1")) == 0)
					{
						std::cout << "\n\n" << imageResponse.to_string().c_str() << "\n\n";
						n = write(connfds[i], imageResponse.to_string().c_str(), imageResponse.to_string().length());
					}
					else if (strncmp(msg.c_str(), "GET /secundaria.html HTTP/1.1", strlen("GET /secundaria.html HTTP/1.1")) == 0)
					{
						std::cout << "\n SECUNDARIA \n";
						n = write(connfds[i], htmlSec.to_string().c_str(), htmlSec.to_string().length());
					}
					else if (strncmp(msg.c_str(), "GET /principal.html HTTP/1.1", strlen("GET /principal.html HTTP/1.1")) == 0)
					{
						n = write(connfds[i], htmlPrin.to_string().c_str(), htmlPrin.to_string().length());
					}
					else if (strncmp(msg.c_str(), "GET /index.html HTTP/1.1", strlen("GET /index.html HTTP/1.1")) == 0)
					{
						n = write(connfds[i], htmlResponse.to_string().c_str(), htmlResponse.to_string().length());
					}
					else if (strncmp(msg.c_str(), "POST /index HTTP/1.1", strlen("POST /upload HTTP/1.1")) == 0) 
					{
						n = write(connfds[i], response.to_string().c_str(), response.to_string().length());
					}
					else if (!msg.empty())
					{
						n = write(connfds[i], htmlError.to_string().c_str(), htmlError.to_string().length());
						std::cout << "Not found\n";
						n = 1;
					}

					if (msg.empty())
					{
						std::cout << "msg is empty\n";
						break;
					}
					memset(buff, 0, BUFF_SIZE);
					std::cout << BLUE << msg << RESET << std::endl;
					msg.clear();
					close(connfds[i]);
				}
				std::cout << "end for\n";
			}
			std::cout << "msg: hola\n";
			break;
			//std::cout << BLUE << msg << RESET << std::endl;
			//HttpRequestHandler obj(buff);
			//std::cout << GREEN << buff << RESET << std::endl;
			//std::cout << obj << std::endl;
			
			/*if (strncmp(msg.c_str(), "GET / HTTP/1.1", strlen("GET / HTTP/1.1")) == 0)
			{
				std::cout << "\n index \n";
				n = write(connfds[0], htmlResponse.to_string().c_str(), htmlResponse.to_string().length());
			}
			else if (strncmp(msg.c_str(), "GET /imgs/goku.jpg HTTP/1.1", strlen("GET /imgs/goku.jpg HTTP/1.1")) == 0)
			{
				std::cout << "\n\n" << imageResponse.to_string().c_str() << "\n\n";
				n = write(connfds[0], imageResponse.to_string().c_str(), imageResponse.to_string().length());
			}
			else if (strncmp(msg.c_str(), "GET /secundaria.html HTTP/1.1", strlen("GET /secundaria.html HTTP/1.1")) == 0)
			{
				std::cout << "\n SECUNDARIA \n";
				n = write(connfds[0], htmlSec.to_string().c_str(), htmlSec.to_string().length());
			}
			else if (strncmp(msg.c_str(), "GET /principal.html HTTP/1.1", strlen("GET /principal.html HTTP/1.1")) == 0)
			{
				n = write(connfds[0], htmlPrin.to_string().c_str(), htmlPrin.to_string().length());
			}
			else if (strncmp(msg.c_str(), "GET /index.html HTTP/1.1", strlen("GET /index.html HTTP/1.1")) == 0)
			{
				n = write(connfds[0], htmlResponse.to_string().c_str(), htmlResponse.to_string().length());
			}
			else if (strncmp(msg.c_str(), "POST /index HTTP/1.1", strlen("POST /upload HTTP/1.1")) == 0) 
			{
				n = write(connfds[0], response.to_string().c_str(), response.to_string().length());
			}
			else if (!msg.empty())
			{
				n = write(connfds[0], htmlError.to_string().c_str(), htmlError.to_string().length());
				std::cout << "Not found\n";
				n = 1;
			}

			if (msg.empty())
			{
				std::cout << "msg is empty\n";
				break;
			}
			memset(buff, 0, BUFF_SIZE);
			msg.clear();
			close(connfds[0]);*/
		}
		for (size_t i = 0; i < fds_size; i++)
			{
				std::cout << "fds[" << i << "].fd: " << fds[i].fd << "\n";
				std::cout << "fds[" << i << "].events: " << fds[i].events << "\n";
				std::cout << "fds[" << i << "].revents: " << fds[i].revents << "\n";
				close(fds[i].fd);
			}
		std::cout << "FIN POLL\n";
		//std::cout << BLUE << msg << RESET << std::endl;
		//HttpRequestHandler obj(buff);
		//std::cout << GREEN << buff << RESET << std::endl;
		//std::cout << obj << std::endl;
		/*response.set_status(200);
		response.set_body("text/plain","Recieved!");

		HttpResponse imageResponse;
		imageResponse.set_status(200);
		imageResponse.set_body("image/jpeg",readImageFile("imgs/goku.jpg"));

		HttpResponse htmlResponse;
		htmlResponse.set_status(200);
		htmlResponse.set_body("text/html",readFile("html/index.html"));

		HttpResponse htmlPrin;
		htmlPrin.set_status(200);
		htmlPrin.set_body("text/html",readFile("html/principal.html"));

		HttpResponse htmlSec;
		htmlSec.set_status(200);
		htmlSec.set_body("text/html",readFile("html/secundaria.html"));

		HttpResponse htmlUpload;
		htmlUpload.set_status(200);
		htmlUpload.set_body("text/html",readFile("html/upload.html"));

		HttpResponse htmlError;
		htmlError.set_status(404);
		htmlError.set_body("text/html",readFile("html/error.html"));

		if (strncmp(msg.c_str(), "GET / HTTP/1.1", strlen("GET / HTTP/1.1")) == 0)
		{
			std::cout << "\n index \n";
			n = write(connfd, htmlResponse.to_string().c_str(), htmlResponse.to_string().length());
		}
		else if (strncmp(msg.c_str(), "GET /imgs/goku.jpg HTTP/1.1", strlen("GET /imgs/goku.jpg HTTP/1.1")) == 0)
		{
			std::cout << "\n\n" << imageResponse.to_string().c_str() << "\n\n";
			n = write(connfd, imageResponse.to_string().c_str(), imageResponse.to_string().length());
		}
		else if (strncmp(msg.c_str(), "GET /secundaria.html HTTP/1.1", strlen("GET /secundaria.html HTTP/1.1")) == 0)
		{
			std::cout << "\n SECUNDARIA \n";
			n = write(connfd, htmlSec.to_string().c_str(), htmlSec.to_string().length());
		}
		else if (strncmp(msg.c_str(), "GET /principal.html HTTP/1.1", strlen("GET /principal.html HTTP/1.1")) == 0)
		{
			n = write(connfd, htmlPrin.to_string().c_str(), htmlPrin.to_string().length());
		}
		else if (strncmp(msg.c_str(), "GET /index.html HTTP/1.1", strlen("GET /index.html HTTP/1.1")) == 0)
		{
			n = write(connfd, htmlResponse.to_string().c_str(), htmlResponse.to_string().length());
		}
		else if (strncmp(msg.c_str(), "POST /index HTTP/1.1", strlen("POST /upload HTTP/1.1")) == 0) 
		{
			n = write(connfd, response.to_string().c_str(), response.to_string().length());
		}
		else if (strncmp(msg.c_str(), "POST /upload HTTP/1.1", strlen("POST /upload HTTP/1.1")) == 0) 
		{*/
			/*std::string boundaryTag = "boundary=";
			size_t boundaryPos = msg.find(boundaryTag);
			std::string boundary = msg.substr(boundaryPos + boundaryTag.length());
			size_t boundaryEndPos = msg.find("\r\n", boundaryPos);*/

			/*if (boundaryPos != std::string::npos) {
				std::string boundaryDelimiter = "--" + msg.substr(boundaryPos + boundaryTag.length(), boundaryEndPos - (boundaryPos + boundaryTag.length()));
				std::cout << "\n\nBoundary MIO: " << boundaryDelimiter << "\n\n";
				std::string body = msg.substr(msg.find(boundaryDelimiter) + boundaryDelimiter.length() + 2);
				std::cout << "\n\nBody MIO: " << body << "\n\n";
				std::string filename;
				std::string contentType;
				size_t filenameStart = body.find("filename=\"");
				size_t filenameEnd = body.find("\"", filenameStart + 10);
				if (filenameStart != std::string::npos && filenameEnd != std::string::npos) {
					filename = body.substr(filenameStart + 10, filenameEnd - (filenameStart + 10));
					std::cout << "Nombre del archivo: " << filename << std::endl;
				}
				// Buscar el tipo de contenido
				size_t contentTypeStart = body.find("Content-Type: ");
				size_t contentTypeEnd = body.find("\n", contentTypeStart);
				if (contentTypeStart != std::string::npos && contentTypeEnd != std::string::npos) {
					contentType = body.substr(contentTypeStart + 14, contentTypeEnd - (contentTypeStart + 14));
					std::cout << "Tipo de contenido: " << contentType << std::endl;
				}
				size_t binaryDataStart = body.find("\r\n\r\n");
				std::string binaryData;
				if (binaryDataStart != std::string::npos) 
				{
					binaryData = body.substr(binaryDataStart + 4);
				}
				std::cout << "Binary data: " << binaryData << std::endl;
				std::cout << "Hexadecimal representation of binary data:\n";
				for (char c : binaryData) {
					printf("%02X ", static_cast<unsigned char>(c));
				}
				std::cout << "\n";
				std::ofstream outputFile("./upload/" + filename, std::ios::binary);
				outputFile.write(binaryData.c_str(), binaryData.size());
				outputFile.close();
				n = write(connfd, htmlUpload.to_string().c_str(), htmlUpload.to_string().length());


			} else {
				std::cout << "Boundary not found\n";
			}*/
			//std::cout << "FILE RECIEVED\n";
		//}
		/*else
		{
			n = write(connfd, htmlError.to_string().c_str(), htmlError.to_string().length());
			std::cout << "Not found\n";
			n = 1;
		}
			//n = write(connfd, response.to_string().c_str(), response.to_string().length());

		if (n < 0)
		{
			std::cout << "[SERVER] Error writing to socket\n";
			return 1;
		}*/
		/*n = write(connfd, htmlResponse.to_string().c_str(), htmlResponse.to_string().length());
		if (n < 0)
		{
			std::cout << "[SERVER] Error writing to socket\n";
			return 1;
		}*/

		/*n = write(connfd, imageResponse.to_string().c_str(), imageResponse.to_string().length());
		if (n < 0)
		{
			std::cout << "[SERVER] Error writing to socket\n";
			return 1;
		}*/

		/*n = write(connfd, response.to_string().c_str(), response.to_string().length());
		if (n < 0)
		{
			std::cout << "[SERVER] Error writing to socket\n";
			return 1;
		}*/
		//memset(buff, 0, BUFF_SIZE);
		//close(connfd);
	}
	return 0;
}
