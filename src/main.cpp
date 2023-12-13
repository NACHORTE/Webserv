#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include "response.hpp"
#include <fstream>
#include <sstream>
#include "defines.h"

#define RED "\033[1;31m"
#define RESET "\033[0m"


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

std::string readImageFile(const std::string& filePath) {
    std::ifstream imageFile(filePath, std::ios::binary);

    if (!imageFile) {
        std::cerr << "Error opening image file: " << filePath << std::endl;
        return "";
    }

    std::ostringstream imageContent;
    imageContent << imageFile.rdbuf();
    return imageContent.str();
}

std::string readHtmlFile(const std::string& filePath) {
    std::ifstream htmlFile(filePath);

    if (!htmlFile) {
        std::cerr << "Error opening HTML file: " << filePath << std::endl;
        return "";
    }

    std::ostringstream htmlContent;
    htmlContent << htmlFile.rdbuf();
    return htmlContent.str();
}

int main()
{
	struct sockaddr_in servaddr, client;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 29\n\nHello from server!";
	HttpResponse response;

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
	while(1)
	{
		std::cout << "[" << millis() << "] SERVER WAITING\n";

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
		std::cout << buff << RED "$\n~~~~~~~~~~~~~~~\n" RESET;
		response.set_status(200);
		response.set_content_type("text/plain");
		response.set_body("Hello from server!");
		response.set_content_len(response.get_length());

		HttpResponse imageResponse;
		imageResponse.set_status(200);
		imageResponse.set_content_type("image/jpeg");
		imageResponse.set_body(readImageFile("imgs/goku.jpg"));
		imageResponse.set_content_len(imageResponse.get_length());

		HttpResponse htmlResponse;
		htmlResponse.set_status(200);
		htmlResponse.set_content_type("text/html");
		htmlResponse.set_body(readHtmlFile("html/index.html"));
		htmlResponse.set_content_len(htmlResponse.get_length());

		HttpResponse htmlUpload;
		htmlUpload.set_status(200);
		htmlUpload.set_content_type("text/html");
		htmlUpload.set_body(readHtmlFile("html/upload.html"));
		htmlUpload.set_content_len(htmlUpload.get_length());

		HttpResponse htmlError;
		htmlError.set_status(404);
		htmlError.set_content_type("text/html");
		htmlError.set_body(readHtmlFile("html/error.html"));
		htmlError.set_content_len(htmlError.get_length());

		if (strncmp(buff, "GET / HTTP/1.1", strlen("GET / HTTP/1.1")) == 0)
			n = write(connfd, htmlResponse.get_response().c_str(), htmlResponse.get_response().length());
		else if (strncmp(buff, "GET /imgs/goku.jpg HTTP/1.1", strlen("GET /imgs/goku.jpg HTTP/1.1")) == 0)
		{
			std::cout << "\n\n" << imageResponse.get_response().c_str() << "\n\n";
			n = write(connfd, imageResponse.get_response().c_str(), imageResponse.get_response().length());
		}
		else if (strncmp(buff, "POST /upload HTTP/1.1", strlen("POST /upload HTTP/1.1")) == 0) 
		{
			std::string boundaryTag = "boundary=";
			size_t boundaryPos = std::string(buff).find(boundaryTag);
			std::string boundary = std::string(buff).substr(boundaryPos + boundaryTag.length());
			size_t boundaryEndPos = std::string(buff).find("\r\n", boundaryPos);

			if (boundaryPos != std::string::npos) {
				std::string boundaryDelimiter = "--" + std::string(buff).substr(boundaryPos + boundaryTag.length(), boundaryEndPos - (boundaryPos + boundaryTag.length()));
				std::cout << "\n\nBoundary MIO: " << boundaryDelimiter << "\n\n";
				std::string body = std::string(buff).substr(std::string(buff).find(boundaryDelimiter) + boundaryDelimiter.length() + 2);
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
				std::ofstream outputFile("./upload/" + filename, std::ios::binary);
				outputFile.write(binaryData.c_str(), binaryData.size());
				outputFile.close();
				n = write(connfd, htmlUpload.get_response().c_str(), htmlUpload.get_response().length());

			} else {
				std::cout << "Boundary not found\n";
			}
			std::cout << "FILE RECIEVED\n";
		}
		else
		{
			n = write(connfd, htmlError.get_response().c_str(), htmlError.get_response().length());
			std::cout << "Not found\n";
			n = 1;
		}
			//n = write(connfd, response.get_response().c_str(), response.get_response().length());

		if (n < 0)
		{
			std::cout << "[SERVER] Error writing to socket\n";
			return 1;
		}
		/*n = write(connfd, htmlResponse.get_response().c_str(), htmlResponse.get_response().length());
		if (n < 0)
		{
			std::cout << "[SERVER] Error writing to socket\n";
			return 1;
		}*/

		/*n = write(connfd, imageResponse.get_response().c_str(), imageResponse.get_response().length());
		if (n < 0)
		{
			std::cout << "[SERVER] Error writing to socket\n";
			return 1;
		}*/

		/*n = write(connfd, response.get_response().c_str(), response.get_response().length());
		if (n < 0)
		{
			std::cout << "[SERVER] Error writing to socket\n";
			return 1;
		}*/
		memset(buff, 0, BUFF_SIZE);
		close(connfd);
	}
	return 0;
}
