#include "CGI.hpp"
#include <unistd.h>

CGI::CgiClient::CgiClient():
    _client(NULL), _pid(-1), _fdOut(-1), _fdIn(-1)
{
    _outOffset = 0;
    _isDone = false;
	_inputBuffer = "HTTP/1.1 200 OK\r\n";
}

CGI::CgiClient::CgiClient(Client &client, int pid, int fdOut, int fdIn):
    _client(&client), _pid(pid), _fdOut(fdOut), _fdIn(fdIn)
{
    _isDone = false;
    _outOffset = 0;
    _outputBuffer = _client->getRequest().getBody();
	_inputBuffer = "HTTP/1.1 200 OK\r\n";
}

CGI::CgiClient::CgiClient(const CgiClient &src)
{
    *this = src;
}

CGI::CgiClient::~CgiClient()
{
}

CGI::CgiClient &CGI::CgiClient::operator=(const CgiClient &rhs)
{
    if (this != &rhs)
    {
        _inputBuffer = rhs._inputBuffer;
        _outputBuffer = rhs._outputBuffer;
        _outOffset = rhs._outOffset;
        _client = rhs._client;
        _pid = rhs._pid;
        _fdOut = rhs._fdOut;
        _fdIn = rhs._fdIn;
    }
    return *this;
}

bool CGI::CgiClient::operator==(const CgiClient &rhs) const
{
    return _client == rhs._client;
}

bool CGI::CgiClient::operator<(const CgiClient &rhs) const
{
    return _client < rhs._client;
}

bool CGI::CgiClient::operator==(const Client &rhs) const
{
    return _client == &rhs;
}

bool CGI::CgiClient::operator<(const Client &rhs) const
{
    return _client < &rhs;
}

int CGI::CgiClient::write(size_t buff_size)
{
    size_t write_size = _outputBuffer.size() - _outOffset;
    if (write_size > buff_size)
        write_size = buff_size;
    if (write_size == 0)
        return 0;
    ssize_t bytesWritten = ::write(_fdOut, _outputBuffer.c_str() + _outOffset, write_size);
    if (bytesWritten != -1)
        _outOffset += bytesWritten;
    return bytesWritten;
}

int CGI::CgiClient::read(size_t buff_size)
{
    std::string buffer(buff_size, '\0');
    ssize_t bytesRead = ::read(_fdIn, &buffer[0], buff_size);
    if (bytesRead > 0)
        _inputBuffer.append(buffer,0,bytesRead);
    return bytesRead;
}

bool CGI::CgiClient::outBufferEmpty() const
{
	return _outOffset == _outputBuffer.size();
}