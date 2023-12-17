#pragma once
#include <iostream>
#include <map>
#include "HttpResponse.hpp"

class HttpRequestHandler
{
	public:
		HttpRequestHandler(void);
		HttpRequestHandler(const std::string &msg);
		HttpRequestHandler(const HttpRequestHandler & src);
		~HttpRequestHandler();
		HttpRequestHandler & operator=(const HttpRequestHandler & rhs);

		std::string command;
		std::string path;
		std::string httpVersion;
		std::map<std::string, std::string> headers;
		std::string body;
		HttpResponse response;
		void parseMsg(const std::string &msg);
	protected:
	private:
		void generateResponse();
		void setHtmlBodyFromFile(const std::string &filename);
	friend std::ostream &operator<<(std::ostream &os, const struct HttpRequestHandler &obj);
};

std::ostream &operator<<(std::ostream &os, const struct HttpRequestHandler &obj);
