#include "Webserv.hpp"

Webserv::Webserv(void)
{
}

Webserv::Webserv(const Webserv & src)
{
	*this = src;
}

Webserv::~Webserv()
{
}

Webserv &Webserv::operator=(const Webserv &rhs)
{
	if (this != &rhs)
	{
		// copy
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Webserv &obj)
{
	(void)obj;
	return (os);
}

void Webserv::init(const std::string &configFile)
{
	// Read config file
	std::vector<Server> servers = read_config(configFile);

	// Add servers to listeners or create new listeners
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		bool added = 0;
		for (std::vector<Listener>::iterator it2 = _listeners.begin(); it2 != _listeners.end(); it2++)
		{
			if (it->getPort() == it2->getPort())
			{
				it2->addServer(*it);
				added = 1;
				break;
			}
		}
		if (!added)
		{
			Listener listener(it->getPort());
			listener.addServer(*it);
			_listeners.push_back(listener);
		}
	}
}
