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

void Client::addData(const std::string & data)
{
	requestBuffer += data;

	// If the header is not over, check if it is complete
	if (!isHeaderComplete)
	{
		size_t headerEnd = requestBuffer.find("\r\n\r\n");

		// Check if the header is too big
		if (headerEnd == std::string::npos && requestBuffer.size() > MAX_HEADER_SIZE)
			Error = true;

		// Check if the header is complete
		if (headerEnd != std::string::npos)
		{
			isHeaderComplete = true;
			// Find the host and save it (if it exists)
			setHost();
			// Check if the body is chunked
			checkBodyType();
			// Check if the body is complete
			if (chunked)
				checkChunkedBody();
			else
				checkBody();
		}
	}
	else
	{
		// Check if the body is too big
		if (requestBuffer.size() > MAX_HEADER_SIZE + MAX_BODY_SIZE)
			Error = true;

		// CHeck if the body is complete
		if (chunked)
			checkChunkedBody();
		else
			checkBody();
	}
}

Client &Client::operator=(const Client &rhs)
{
	if (this != &rhs)
	{
		// copy
	}
	return (*this);
}

void Client::setHost(void)
{
	size_t hostStart = requestBuffer.find("Host:");
	if (hostStart == std::string::npos)
		return;
	hostStart += 5;
	size_t hostEnd = requestBuffer.find("\n", hostStart);
	host = trim(requestBuffer.substr(hostStart, hostEnd - hostStart));
}

std::ostream &operator<<(std::ostream &os, const Client &obj)
{
	(void)obj;
	return (os);
}
