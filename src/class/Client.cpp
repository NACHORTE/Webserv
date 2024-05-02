#include "Client.hpp"
#include "defines.h"
#include "utils.hpp"
#include "colors.h"

Client::Client(std::string IP, int port)
{
	_Error = false;
	_lastEventTime = clock();
	_IP = IP;
	_port = port;
}

Client::Client(const Client & src)
{
	*this = src;
}

Client::~Client()
{}

void Client::setResponse(const HttpResponse & response)
{
	if (_requests.empty())
		return;
	_requests.begin()->second = response;
}

std::string Client::getIP() const
{
	return (_IP);
}

int Client::getPort() const
{
	return (_port);
}

std::string Client::getHost(void) const
{
	if (_requests.empty())
		return ("");

	std::vector<std::string> header = _requests.rbegin()->first.getHeader("Host");
	if (header.empty())
		return ("");
	return (header[0]);
}

std::string Client::getResponse() const
{
	if (_requests.empty())
		return ("");
	return (_requests.rbegin()->second());
}

const HttpRequest &Client::getRequest(void) //TODO hacer esto de otra forma
{
	if (_requests.empty())
		throw std::runtime_error("No request available");
	return (_requests.rbegin()->first);
}

size_t Client::getRequestCount(void) const
{
	return (_requests.size());
}

void Client::addData(const std::string & data)
{
	// Update the last event time
	_lastEventTime = clock();

	size_t bytesRead = 0;
	while (bytesRead < data.size())
	{
		if (_requests.empty() || _requests.begin()->first.requestReady())
			_requests.push_front(std::pair<HttpRequest, HttpResponse>());
		size_t read = _requests.begin()->first.addData(data.substr(bytesRead));
		if (read == 0)
			break;
		bytesRead += read;
	}
}

bool Client::requestReady() const
{
	if (_requests.empty())
		return (false);
	return (_requests.begin()->first.requestReady());
}

bool Client::responseReady() const
{
	if (_requests.empty())
		return (false);
	return (_requests.begin()->second.responseReady());
}

bool Client::timeout() const
{
	// Calculate the time since the last event
	double seconds = (double)(clock() - _lastEventTime) / CLOCKS_PER_SEC;
	// Return true if the time since the last event is greater than the timeout
	return (seconds > TIMEOUT);
}

bool Client::keepAlive() const
{
	if (_requests.empty())
		return (false);
	std::vector<std::string> header = _requests.begin()->first.getHeader("Connection");
	if (header.empty())
		return (false);
	return (header[0] == "keep-alive");
}

void Client::popRequest()
{
	_lastEventTime = clock();

	if (!_requests.empty())
		_requests.pop_back();
}

Client &Client::operator=(const Client &rhs)
{
	if (this != &rhs)
	{
		_IP = rhs._IP;
		_port = rhs._port;
		_Error = rhs._Error;
		_lastEventTime = rhs._lastEventTime;
		_requests = rhs._requests;
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Client &obj)
{
	std::cout << CYAN << "_Error: "<< RESET << obj._Error << std::endl;
	std::cout << CYAN << "_lastEventTime: "<< RESET << obj._lastEventTime << std::endl;
	std::cout << CYAN << "_requests: "<< RESET << obj._requests.size() << std::endl;

	std::list<std::pair<HttpRequest, HttpResponse> >::const_iterator it;
	size_t i = 1;
	for (it = obj._requests.begin(); it != obj._requests.end(); it++)
	{
		std::cout << (it->first.requestReady()?GREEN:RED);
		std::cout << "Request " << i << (it->first.requestReady()?" ready":" not ready") << "\n[" << RESET;
		std::cout << it->first;
		std::cout << (it->first.requestReady()?GREEN:RED)<< "]" << RESET << std::endl;
		std::cout << (it->second.responseReady()?GREEN:RED);
		std::cout << "Response " << i << (it->second.responseReady()?" ready":" not ready") << "\n[" << RESET;
		std::cout << it->second;
		std::cout << (it->second.responseReady()?GREEN:RED)<< "]" << RESET;
		++i;
	}
	if (obj._requests.size() == 0)
		std::cout << RED << "No requests" << RESET;
	return (os);
}
