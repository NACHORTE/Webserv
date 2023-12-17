#include "HttpRequestHandler.hpp"
#include "defines.h"
#include <unistd.h>
#include <cstring>
#include <sstream>
#include "colors.h"

HttpRequestHandler::HttpRequestHandler(void):
	command(), path(), httpVersion(), headers(), body()
{}

HttpRequestHandler::HttpRequestHandler(const std::string &msg)
{
	parseMsg(msg);
}

HttpRequestHandler::HttpRequestHandler(const HttpRequestHandler & src):
	command(src.command), path(src.path), httpVersion(src.httpVersion),
	headers(src.headers), body(src.body)
{}

HttpRequestHandler::~HttpRequestHandler()
{}

HttpRequestHandler &HttpRequestHandler::operator=(const HttpRequestHandler &rhs)
{
	if (this != &rhs)
	{
		command = rhs.command;
		path = rhs.path;
		httpVersion = rhs.httpVersion;
		headers = rhs.headers;
		body = rhs.body;
	}
	return (*this);
}

void HttpRequestHandler::parseMsg(const std::string &msg)
{
    // Using stringstream to read the msg string
    std::stringstream ss(msg); 
    std::string line;

    // Read first line and save Command, Path and httpversion
    std::getline(ss, line);
    std::istringstream lineStream(line);
    lineStream >> command >> path >> httpVersion;

    // Save headers
    while (std::getline(ss, line) && !line.empty())
	{
        size_t separator = line.find(":");
        if (separator != std::string::npos) {
            std::string key = line.substr(0, separator);
            std::string value = line.substr(separator + 2);
			std::cout << (int)value[value.length() - 1]  << (int)'\n'<< "\n";
			if (!value.empty() && value[value.length() - 1] == '\n')
			{
				std::cout << "popping value of " << value << "\n";
    			value.pop_back();
			}
			headers[key] += value;
        }
    }

	//save the body
    std::stringstream bodyStream;
	bodyStream << ss.rdbuf();
	body = bodyStream.str();
}

std::ostream &operator<<(std::ostream &os, const HttpRequestHandler &obj)
{
	os << GREEN "First line content:"
			<< CYAN "\n\t_command: " RESET << "\"" <<obj.command << "\""
			<< CYAN "\n\t_path: " RESET << "\"" << obj.path << "\""
			<< CYAN "\n\t_httpversion: " RESET << "\"" << obj.httpVersion << "\""
			<< "\n";
	os << GREEN "Header content:" RESET "\n";
	std::map<std::string, std::string>::const_iterator it;
 	for (it = obj.headers.begin(); it != obj.headers.end(); ++it) {
        os << CYAN "\tKey: \"" RESET << it->first << CYAN "\"    Value: \"" RESET << it->second << CYAN "\"" RESET << std::endl;
    }
		os << GREEN "Body content:" RESET "\n\t\""<< obj.body << "\"\n";
	return (os);
}
