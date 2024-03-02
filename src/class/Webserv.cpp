#include "Webserv.hpp"

Webserv::Webserv(void)
{}

Webserv::~Webserv()
{}

void Webserv::init(const std::string &configFile)
{
	// Read config file
	std::vector<Server> servers = read_config(configFile);

	// Add servers to listeners or create new listeners
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
		addServer(*it);
}

void Webserv::loop(void)
{
	// Loop through listeners
	// NOTE try catch
	std::vector<Listener>::iterator it;
	for (it = _listeners.begin(); it != _listeners.end(); it++)
		it->loop();
}

void Webserv::addServer(const Server &server)
{
	for (std::vector<Listener>::iterator it = _listeners.begin(); it != _listeners.end(); it++)
	{
		if (it->getPort() == server.getPort())
		{
			it->addServer(server);
			return;
		}
	}
	Listener listener(server.getPort());
	listener.addServer(server);
	_listeners.push_back(listener);
}

std::ostream &operator<<(std::ostream &os, const Webserv &obj)
{
	(void)obj;
	return (os);
}
