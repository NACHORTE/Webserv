#pragma once
#include <iostream>
#include <map>

class HttpRequestHandler
{
	public:
		HttpRequestHandler(void);
		HttpRequestHandler(const HttpRequestHandler & src);
		~HttpRequestHandler();
		HttpRequestHandler & operator=(const HttpRequestHandler & rhs);

		const std::string &getCommand();
		const std::string &getPath();
		const std::string &getHttpVersion();
		const std::string &getHeader(const std::string &key);
		const std::string &getBody();

		void parseMsg(const std::string &msg);
	protected:
	private:
		std::string _command;
		std::string _path;
		std::string _httpVersion;
		std::map<std::string, std::string> _headers;
		std::string _body;
	
	friend std::ostream &operator<<(std::ostream &os, const HttpRequestHandler &obj);
};

std::ostream &operator<<(std::ostream &os, const HttpRequestHandler &obj);
