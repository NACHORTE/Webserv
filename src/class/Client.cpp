#include "Client.hpp"
#include "defines.h"
#include "utils.hpp"

Client::Client(void)
{}

Client::Client(const Client & src)
{
	*this = src;
}

Client::~Client()
{}

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

void Client::addData(const std::string & data)
{
	// Update the last event time
	_lastEventTime = clock();

	size_t bytesRead = 0;
	while (bytesRead < data.size())
	{
		if (_requests.empty() || _requests.begin()->first.requestReady())
			_requests.push_front(std::pair<HttpRequest, HttpResponse>());
		bytesRead += _requests.begin()->first.addData(data.substr(bytesRead));
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

const std::string Client::popResponse(size_t length)
{
	_lastEventTime = clock();

	if (_requests.empty())
		return ("");
	return (_requests.begin()->second.popResponse(length));
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
		// TODO copy
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Client &obj)
{
	(void)obj;
	return (os);
}
