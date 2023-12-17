#pragma once
#include <iostream>
#include <map>

struct HttpRequestHandler
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

		void parseMsg(const std::string &msg);
	protected:
	private:

	friend std::ostream &operator<<(std::ostream &os, const struct HttpRequestHandler &obj);
};

std::ostream &operator<<(std::ostream &os, const struct HttpRequestHandler &obj);
