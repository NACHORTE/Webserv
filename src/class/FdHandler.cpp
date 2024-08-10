#include "FdHandler.hpp"
#include "utils.hpp"
#include <unistd.h>

int FdHandler::_timeout = 0;
std::vector<struct pollfd> FdHandler::_fds;
std::map<int, int> FdHandler::_revents;

FdHandler::FdHandler()
{}

FdHandler::FdHandler(const FdHandler & src)
{
	*this = src;
}

FdHandler::~FdHandler()
{}

FdHandler &FdHandler::operator=(const FdHandler &rhs)
{
	if (this != &rhs)
	{
		_timeout = rhs._timeout;
		_fds = rhs._fds;
		_revents = rhs._revents;
	}
	return (*this);
}

void FdHandler::setTimeout(int timeout)
{
	_timeout = timeout;
}

int FdHandler::getTimeout()
{
	return (_timeout);
}

void FdHandler::addFd(int fd, int events)
{
	// If already in _fds, return
	if (_revents.count(fd) != 0)
		return;

	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = events;
	pfd.revents = 0;
	_fds.push_back(pfd);
	// Don't update _revents here, it will be updated by poll()
}

void FdHandler::removeFd(int fd, bool close)
{
	for (size_t i = 0; i < _fds.size(); ++i)
	{
		if (_fds[i].fd == fd)
		{
			_fds.erase(_fds.begin() + i);
			_revents.erase(fd);
			if (close)
				::close(fd);
			return;
		}
	}
}

int FdHandler::getRevents(int fd)
{
	if (_revents.count(fd) == 0)
		return (0);
	return (_revents.at(fd));
}

void FdHandler::poll()
{
	// Reset revents
	_revents.clear();

	// Poll
	int ret = ::poll(&_fds[0], _fds.size(), _timeout);
	if (ret == -1)
	{
		DEBUG("poll failed");
		return;
	}
	// Update revents
	for (size_t i = 0; i < _fds.size(); ++i)
		if (_fds[i].revents != 0)
			_revents[_fds[i].fd] = _fds[i].revents;
}

void FdHandler::clear(bool close)
{
	if (close)
		for (size_t i = 0; i < _fds.size(); ++i)
			::close(_fds[i].fd);
	_fds.clear();
	_revents.clear();
}

std::ostream &operator<<(std::ostream &os, const FdHandler &obj)
{
	os << "FdHandler:\n\t_timeout: " << obj._timeout << "\n\t_fds:" << std::endl;
	for (std::vector<struct pollfd>::const_iterator it = obj._fds.begin(); it != obj._fds.end(); ++it)
		os << "\t\tfd: " << it->fd << ", events: " << it->events << ", revents: " << it->revents << std::endl;
	return (os);
}
