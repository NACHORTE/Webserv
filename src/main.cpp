#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#define BUFF_SIZE 255
#define SERV_HOST_ADDR "127.0.0.1"
#define SERV_PORT 8080
#define BACKLOG 5

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

int main()
{
	struct sockaddr_in servaddr, client;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 29\n\nLogi eres un muerto de hambre";
	if (sockfd == -1)
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
	}
	unsigned long last_time = millis();
	while(1)
	{
		unsigned long this_time = millis();
		if (this_time - last_time > 1000)
		{
			std::cout << "[" << this_time << "] SERVER WAITING\n"; 
			last_time = this_time;
		}
		unsigned int len = sizeof(client);
		int connfd = accept(sockfd, (struct sockaddr *)&client, &len);
		if (connfd < 0)
		{
			std::cout << "[SERVER] Error accept\n";
			return 1;
		}
		std::cout << "new_client!\n";
		char buff[BUFF_SIZE];
		int n = read(connfd, buff, BUFF_SIZE);
		if (n < 0)
		{
			std::cout << "[SERVER] Error reading from socket\n";
			return 1;
		}
		std::cout << buff;
		n = write(connfd, hello, strlen(hello));
		if (n < 0)
		{
			std::cout << "[SERVER] Error writing to socket\n";
			return 1;
		}
		memset(buff, 0, BUFF_SIZE);
		close(connfd);
	}
	return 0;
}
