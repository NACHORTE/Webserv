#include "CGI.hpp"
#include <list>
#include "Server.hpp"
#include "stdlib.h"
#include "unistd.h"
#include "sys/wait.h"
#include "utils.hpp"

CGI::CGI(void)
{}

CGI::CGI(const CGI & src)
{
	*this = src;
}

CGI::~CGI()
{}

CGI &CGI::operator=(const CGI &rhs)
{
	if (this != &rhs)
	{
        _pollfd = rhs._pollfd;
        _clients = rhs._clients;
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const CGI &obj)
{
	os << "CGI: " << obj._pollfd.size() << " fds, " << obj._clients.size() << " clients" << std::endl;
	for (size_t i = 0; i < obj._pollfd.size(); ++i)
		os << "[" << i << "] "<<obj._pollfd[i].fd << std::endl;
	for (size_t i = 0; i < obj._clients.size(); ++i)
	{
		os << "[" << i << "] fdin "<<obj._clients[i]._fdIn << std::endl;
		os << "[" << i << "] fdout"<<obj._clients[i]._fdOut << std::endl;
	}
	return (os);
}

// Defined in newCgi.cpp
//void CGI::newCgi(Client &client, const std::string &filename, const Server &server);

void CGI::closeCgi(Client &client)
{    
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        if (_clients[i]._client == &client)
        {
            close(_clients[i]._fdIn);
            close(_clients[i]._fdOut);
            kill(_clients[i]._pid, SIGKILL);
            _clients.erase(_clients.begin() + i);
            _pollfd.erase(_pollfd.begin() + i * 2);
            _pollfd.erase(_pollfd.begin() + i * 2);
            return;
        }
    }
}

void CGI::closeCgi(size_t index)
{
    if (index >= _clients.size())
        return;
    close(_clients[index]._fdIn);
    close(_clients[index]._fdOut);
    kill(_clients[index]._pid, SIGKILL);
    _clients.erase(_clients.begin() + index);
    _pollfd.erase(_pollfd.begin() + index * 2);
    _pollfd.erase(_pollfd.begin() + index * 2);
}

void CGI::loop(const Server &server)
{
	// Check if the CGI programs have finished and reset _somethingToRead flag
    for (size_t i = 0; i < _clients.size(); ++i)
    {
		_clients[i]._somethingToRead = false;

		if(_clients[i]._isDone)
			continue;

        int status;
        pid_t result = waitpid(_clients[i]._pid, &status, WNOHANG);
		// If there is an error with waitpid, close the CGI program
        if (result == -1)
        {
            _clients[i]._client->setResponse(server.errorResponse(500, "internal_server_error", "waitpid failed"));
            closeCgi(i--);
        }
		// If the CGI program has finished
        else if (result > 0)
        {
            if (WIFEXITED(status))
            {
                if (WEXITSTATUS(status) == EXIT_FAILURE)
                {
                    _clients[i]._client->setResponse(server.errorResponse(500, "internal_server_error", "cgi exited with failure"));
                    closeCgi(i--);
                }
                else
                    _clients[i]._isDone = true;
            }
            else if (WIFSIGNALED(status))
            {
                _clients[i]._client->setResponse(server.errorResponse(500, "internal_server_error", "cgi terminated by signal"));
                closeCgi(i--);
            }
        }
    }

    // Check if there is something to read or write to the CGI programs
    if (poll(&_pollfd[0], _pollfd.size(), POLL_TIMEOUT) > 0)
    {
        for (size_t i = 0; i < _pollfd.size(); ++i)
        {
			size_t clientIndex = i / 2;
            if (_pollfd[i].revents & POLLIN)
            {
				_clients[clientIndex]._somethingToRead = true;
                if (_clients[clientIndex].read(IO_BUFF_SIZE) == -1)
                {
                    _clients[clientIndex]._client->setResponse(server.errorResponse(500, "internal_server_error", "couldn't read from cgi"));
                    closeCgi(clientIndex);
					i = clientIndex * 2 - 1;
                }
            }
            else if (_pollfd[i].revents & POLLERR or _pollfd[i].revents & POLLHUP)
            {
				generateResponse(_clients[clientIndex], server);
				closeCgi(clientIndex);
				i = clientIndex * 2 - 1;
            }
            else if (_pollfd[i].revents & POLLOUT and not _clients[clientIndex].outBufferEmpty())
            {
                if (_clients[clientIndex].write(IO_BUFF_SIZE) == -1)
                {
                    _clients[clientIndex]._client->setResponse(server.errorResponse(500, "internal_server_error", "couldn't write to cgi"));
                    closeCgi(clientIndex);
					i = clientIndex * 2 - 1;
                }
            }
        }
    }

	// Disconnect all clients that have finished and have no more data to read
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i]._isDone and not _clients[i]._somethingToRead)
		{
			generateResponse(_clients[i], server);
			closeCgi(i--);
		}
	}
}

#include "colors.h"

void CGI::generateResponse(CgiClient &cgiClient, const Server &server)
{
	HttpResponse & response = cgiClient._client->getResponse();

	// get the position of the end of the header
	size_t endOfHeader = cgiClient._inputBuffer.find("\r\n\r\n");
	if (endOfHeader == std::string::npos)
	{
		response = server.errorResponse(500, "internal_server_error", "cgi didn't send a complete response");
		return;
	}
	endOfHeader += 4;
	std::string header = cgiClient._inputBuffer.substr(0, endOfHeader);
	std::string body = cgiClient._inputBuffer.substr(endOfHeader);

	// Parse the header
	response.addData(header);
	// If there is a Status header, set the status code and phrase and remove the header
	std::vector<std::string> statusHeader = response.getHeader("Status");
	if (statusHeader.size() > 0)
	{
		std::istringstream iss(statusHeader[0]);
		int status;
		iss >> status;
		std::string phrase;
		std::getline(iss, phrase);
		response.setStatus(status, trim(phrase));
		response.unsetHeader("Status");
	}

	// Add the body to the response
	std::vector<std::string> contentLength = response.getHeader("Content-Length");
	if (contentLength.size() == 0)
		response.setHeader("Content-Length", intToString(body.size()));
	response.responseReady(false);
	response.addData(body);

	// If the response is not ready after adding all the data from cgi, send an error response
	if (not response.responseReady())
	{
		response.clear();
		response = server.errorResponse(500, "internal_server_error", "cgi didn't send a complete response");
	}
}