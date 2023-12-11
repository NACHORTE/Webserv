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

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr;
	char buff[BUFF_SIZE];
	int n;

	// Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		std::cout << "Error creating socket" << std::endl;
		return -1;
	}

	// Set server address
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, SERV_HOST_ADDR, &serv_addr.sin_addr);

	// Connect to server
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cout << "Error connecting to server" << std::endl;
		return -1;
	}

	// Send message to server
	std::cout << "Enter message to send to server: ";
	memset(buff, 0, BUFF_SIZE);
	fgets(buff, BUFF_SIZE, stdin);
	n = write(sockfd, buff, strlen(buff));
	if (n < 0)
	{
		std::cout << "Error writing to socket" << std::endl;
		return -1;
	}

	// Read message from server
	memset(buff, 0, BUFF_SIZE);
	n = read(sockfd, buff, BUFF_SIZE);
	if (n < 0)
	{
		std::cout << "Error reading from socket" << std::endl;
		return -1;
	}
	std::cout << "Message from server: " << buff << std::endl;

	// Close socket
	close(sockfd);

	return 0;
}
