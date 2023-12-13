#include "HttpRequestHandler.hpp"
#include "defines.h"
#include <unistd.h>
#include <cstring>
#include <sstream>

HttpRequestHandler::HttpRequestHandler(void)
{
}

HttpRequestHandler::HttpRequestHandler(const HttpRequestHandler & src)
{
	*this = src;
}

HttpRequestHandler::~HttpRequestHandler()
{
}

const std::string &HttpRequestHandler::getCommand()
{
	return _command;
}

const std::string &HttpRequestHandler::getPath()
{
	return _path;
}

const std::string &HttpRequestHandler::getHttpVersion()
{
	return _httpVersion;
}

const std::string &HttpRequestHandler::getHeader(const std::string &key)
{
	return _headers[key];
}

const std::string &HttpRequestHandler::getBody()
{
	return _body;
}


HttpRequestHandler &HttpRequestHandler::operator=(const HttpRequestHandler &rhs)
{
	if (this != &rhs)
	{
		// copy
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
    lineStream >> _command >> _path >> _httpVersion;

    // Save headers
    while (std::getline(ss, line) && !line.empty())
	{
        size_t separator = line.find(":");
        if (separator != std::string::npos) {
            std::string key = line.substr(0, separator);
            std::string value = line.substr(separator + 2);  // +2 to skip ": "
            _headers[key] += value; // += so if the 
        }
    }

	//save the body
    std::stringstream bodyStream;
	bodyStream << ss.rdbuf();
	_body = bodyStream.str();
}

std::ostream &operator<<(std::ostream &os, const HttpRequestHandler &obj)
{

	std::map<std::string, std::string>::iterator iterador;
    for (iterador = obj._headers.begin(); iterador != miMapa.end(); ++iterador) {
        // Acceder a la clave y al valor usando el iterador
        std::cout << "Clave: " << iterador->first << ", Valor: " << iterador->second << std::endl;
    }
	return (os);
}
