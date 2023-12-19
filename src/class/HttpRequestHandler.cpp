#include "HttpRequestHandler.hpp"
#include "defines.h"
#include <unistd.h>
#include <sstream>
#include <fstream>
#include "utils.hpp"
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
	generateResponse();
}

void HttpRequestHandler::setHtmlBodyFromFile(const std::string &filename)
{
	try
	{
		response.set_status(200);
		response.set_body("text/html",readHtmlFile(filename));
	}
	catch (const std::invalid_argument &e) 
	{
		response.set_status(404);
		try
		{
			response.set_body("text/html",readHtmlFile("html/error.html"));
		}
		catch (...)
		{
			response.set_body("text/plain","Error opening HTML file (file not found)");
		}
	}
	catch (const std::runtime_error &e)
	{
		response.set_status(500);
		response.set_body("text/plain",std::string("Error opening HTML file (other error): ") + e.what());
	}
}

void HttpRequestHandler::generateResponse()
{
	std::string command_list[] = {"GET", "POST", "DELETE"};
	std::string allowed_paths[] = {"html", "imgs"};

	size_t num_elements = sizeof(command_list) / sizeof(command_list[0]);
	int command_selected = -1;
	for (size_t i = 0; i < num_elements; i++)
	{
		if (command == command_list[i])
		{
			command_selected = i;
			break;
		}
	}
	if (command_selected == -1)
	{
		response.set_status(400);
		response.set_body("text/plain",std::string("Command \"") + command + std::string("\" couldn't be found"));
		return ;
	}

	if (path == "/")
	{
		setHtmlBodyFromFile("html/index.html");
		return ;
	}
	if (checkPathFormat(path) == false)
	{
		response.set_status(400);
		response.set_body("text/plain",std::string("Path \"") + path + std::string("\" is invalid"));
		return ;
	}
	if (checkAllowedPath(path, allowed_paths) == false)
	{
		response.set_status(403);
		response.set_body("text/plain",std::string("Path \"") + path + std::string("\" is not allowed"));
		return ;
	}
	num_elements = sizeof(allowed_paths) / sizeof(allowed_paths[0]);
	for (size_t i = 0; i < num_elements; i++)
	{
		if (path.find(std::string("/") + allowed_paths[i]) != std::string::npos)
		{
			response.set_status(200);
			setHtmlBodyFromFile(path);
			return ;
		}
	}
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
