#include "Client.hpp"
#include "defines.h"
#include "utils.hpp"

#include <ctime>

Client::Client(std::string IP, int port)
{
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
		_requests.push_front(std::pair<HttpRequest, HttpResponse>());
	_requests.rbegin()->second = response;
	// Update the last event time
	_lastEventTime = clock();
}

void Client::setResponse(const std::string & response)
{
    if (_requests.empty())
        _requests.push_front(std::pair<HttpRequest, HttpResponse>());
    _requests.rbegin()->second.addData(response);
	// Update the last event time
	_lastEventTime = clock();
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

HttpResponse & Client::getResponse()
{
	if (_requests.empty())
		throw std::runtime_error("No response available");
	return (_requests.rbegin()->second);
}

const HttpResponse & Client::getResponseConst() const
{
	if (_requests.empty())
		throw std::runtime_error("No response available");
	return (_requests.rbegin()->second);
}

HttpRequest &Client::getRequest(void)
{
	if (_requests.empty())
		throw std::runtime_error("No request available");
	return (_requests.rbegin()->first);
}

const HttpRequest &Client::getRequestConst(void) const
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
	size_t bytesRead = 0;
	while (bytesRead < data.size())
	{
		if (_requests.empty() || _requests.begin()->first.requestReady())
			_requests.push_front(std::pair<HttpRequest, HttpResponse>());
		long long int read = _requests.begin()->first.addData(data.substr(bytesRead));
		if (read <= 0)
			return;
		bytesRead += read;
	}

	// Update the last event time
	_lastEventTime = clock();
}

bool Client::requestReady() const
{
	if (_requests.empty())
		return (false);
	return (_requests.rbegin()->first.requestReady());
}

bool Client::responseReady() const
{
	if (_requests.empty())
		return (false);
	return (_requests.rbegin()->second.responseReady());
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
	std::vector<std::string> header = _requests.rbegin()->first.getHeader("Connection");
	if (header.empty())
		return (false);
	return (header[0] == "keep-alive");
}

void Client::popRequest()
{
	_lastEventTime = clock();

	if (!_requests.empty())
		_requests.pop_back();

	// Update the last event time
	_lastEventTime = clock();
}

void Client::error(bool error)
{
	_error = error;
}

bool Client::error() const
{
	if (_requests.empty())
		return (false);
	return (_requests.rbegin()->first.error() or _requests.rbegin()->second.error() or _error);
}

Client &Client::operator=(const Client &rhs)
{
	if (this != &rhs)
	{
		_IP = rhs._IP;
		_port = rhs._port;
		_lastEventTime = rhs._lastEventTime;
		_requests = rhs._requests;
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Client &obj)
{
	std::cout << "_lastEventTime: " << obj._lastEventTime << std::endl;
	std::cout << "_requests: " << obj._requests.size() << std::endl;

	std::list<std::pair<HttpRequest, HttpResponse> >::const_iterator it;
	size_t i = 1;
	for (it = obj._requests.begin(); it != obj._requests.end(); it++)
	{
		std::cout << "Request " << i << (it->first.requestReady()?" ready":" not ready") << "\n[";
		std::cout << it->first;
		std::cout << "]" << std::endl;
		std::cout << "Response " << i << (it->second.responseReady()?" ready":" not ready") << "\n[";
		std::cout << it->second;
		std::cout << "]";
		++i;
	}
	if (obj._requests.size() == 0)
		std::cout << "No requests";
	return (os);
}
