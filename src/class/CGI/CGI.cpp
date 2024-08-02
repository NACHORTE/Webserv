#include "CGI.hpp"
#include <list>
#include "Server.hpp"
#include "stdlib.h"
#include "unistd.h"
#include "sys/wait.h"

CGI::CGI(void)
{
}

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
		_pollfdIn = rhs._pollfdIn;
        _pollfdOut = rhs._pollfdOut;
        _clients = rhs._clients;
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const CGI &obj)
{
	(void)obj;
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
            _pollfdIn.erase(_pollfdIn.begin() + i);
            _pollfdOut.erase(_pollfdOut.begin() + i);
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
    _pollfdIn.erase(_pollfdIn.begin() + index);
    _pollfdOut.erase(_pollfdOut.begin() + index);
}

void CGI::loop(const Server &server)
{
    // Check if there is something to read or write to the CGI programs
    if (poll(&_pollfdOut[0], _pollfdOut.size(), POLL_TIMEOUT) > 0
        or poll(&_pollfdIn[0], _pollfdIn.size(), POLL_TIMEOUT) > 0)
    {
        for (size_t i = 0; i < _pollfdOut.size(); ++i)
        {
            bool readReady = _pollfdIn[i].revents & POLLIN;
            bool writeReady = _pollfdOut[i].revents & POLLOUT;
            bool error = _pollfdOut[i].revents & POLLERR or _pollfdIn[i].revents & POLLERR;
            bool disconnected = _pollfdOut[i].revents & POLLHUP or _pollfdIn[i].revents & POLLHUP;
            if (error)
            {
                _clients[i]._client->setResponse(server.errorResponse(500, "internal_server_error", "cgi error"));
                closeCgi(i--);
                continue;
            }
            if (readReady)
            {
                if (_clients[i].read(IO_BUFF_SIZE) == -1)
                {
                    _clients[i]._client->setResponse(server.errorResponse(500, "internal_server_error", "couldn't read from cgi"));
                    closeCgi(i--);
                    continue;
                }
            }
            if (disconnected and not readReady)
            {
                _clients[i]._client->setResponse(_clients[i]._inputBuffer);
                if (not _clients[i]._client->responseReady())
                    _clients[i]._client->setResponse(server.errorResponse(500, "internal_server_error", "cgi didn't send a complete response"));
                closeCgi(i--);
                continue;
            }
            if (writeReady and not _clients[i].outBufferEmpty() and not disconnected)
            {
                if (_clients[i].write(IO_BUFF_SIZE) == -1)
                {
                    _clients[i]._client->setResponse(server.errorResponse(500, "internal_server_error", "couldn't write to cgi"));
                    closeCgi(i--);
                }
            }
        }
    }

    // Check if the CGI programs have finished
    for (size_t i = 0; i < _clients.size(); ++i)
    {
        int status;
        pid_t result = waitpid(_clients[i]._pid, &status, WNOHANG);
        
        if (result == -1)
        {
            _clients[i]._client->setResponse(server.errorResponse(500, "internal_server_error", "waitpid failed"));
            closeCgi(i--);
        }
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
}
