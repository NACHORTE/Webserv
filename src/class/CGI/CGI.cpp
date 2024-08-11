#include "CGI.hpp"
#include <list>
#include "Server.hpp"
#include "stdlib.h"
#include "unistd.h"
#include "sys/wait.h"
#include "utils.hpp"
#include "FdHandler.hpp"

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
        _clients = rhs._clients;
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const CGI &obj)
{
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
            FdHandler::removeFd(_clients[i]._fdIn);
            FdHandler::removeFd(_clients[i]._fdOut);
            kill(_clients[i]._pid, SIGKILL);
            _clients.erase(_clients.begin() + i);
            return;
        }
    }
}

void CGI::closeCgi(size_t index)
{
    if (index >= _clients.size())
        return;
    FdHandler::removeFd(_clients[index]._fdIn);
    FdHandler::removeFd(_clients[index]._fdOut);
    kill(_clients[index]._pid, SIGKILL);
    _clients.erase(_clients.begin() + index);
}

void CGI::loop(Server &server)
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
            server.errorResponse(*(_clients[i]._client), 500, "internal_server_error", "waitpid failed");
            closeCgi(i--);
        }
		// If the CGI program has finished
        else if (result > 0)
        {
            if (WIFEXITED(status))
            {
                if (WEXITSTATUS(status) == EXIT_FAILURE)
                {
                    server.errorResponse(*(_clients[i]._client), 500, "internal_server_error", "cgi exited with failure");
                    closeCgi(i--);
                }
                else
                    _clients[i]._isDone = true;
            }
            else if (WIFSIGNALED(status))
            {
                server.errorResponse(*(_clients[i]._client), 500, "internal_server_error", "cgi terminated by signal");
                closeCgi(i--);
            }
        }
    }

	for (size_t i = 0; i < _clients.size(); ++i)
	{
		// Check inFd, try to read first then check for errors
		int inRevents = FdHandler::getRevents(_clients[i]._fdIn);
		if (inRevents & POLLIN)
		{
			_clients[i]._somethingToRead = true;
			if (_clients[i].read(IO_BUFF_SIZE) == -1)
			{
				server.errorResponse(*(_clients[i]._client), 500, "internal_server_error", "couldn't read from cgi");
				closeCgi(i--);
			}
		}
		else if (inRevents & POLLERR or inRevents & POLLHUP)
		{
			generateResponse(_clients[i], server);
			closeCgi(i--);
		}
		// Check outFd, first check if there is an error or hangup, then check if there is data to write
		int outRevents = FdHandler::getRevents(_clients[i]._fdOut);
		if (outRevents & POLLERR or outRevents & POLLHUP)
		{
			generateResponse(_clients[i], server);
			closeCgi(i--);
		}
		else if (outRevents & POLLOUT and not _clients[i].outBufferEmpty())
		{
			if (_clients[i].write(IO_BUFF_SIZE) == -1)
			{
				server.errorResponse(*(_clients[i]._client), 500, "internal_server_error", "couldn't write to cgi");
				closeCgi(i--);
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



void CGI::generateResponse(CgiClient &cgiClient, Server &server)
{
	HttpResponse & response = cgiClient._client->getResponse();
	response.clear();

	// get the position of the end of the header
	size_t endOfHeader = cgiClient._inputBuffer.find("\r\n\r\n");
	if (endOfHeader == std::string::npos)
	{
		server.errorResponse(*(cgiClient._client), 500, "internal_server_error", "cgi didn't send a complete response");
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
		if (!(iss >> status) or status < 100 or status > 599)
		{
			server.errorResponse(*(cgiClient._client), 500, "internal_server_error", "cgi didn't send a valid status code");
			return;
		}
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
		server.errorResponse(*(cgiClient._client), 500, "internal_server_error", "cgi didn't send a complete response");
	}
}