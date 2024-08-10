#include "Webserv.hpp"
#include "readConfig.hpp"
#include "FdHandler.hpp"
Webserv::Webserv(void)
{}

Webserv::~Webserv()
{
	FdHandler::clear();
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

void Webserv::init(const std::string &configFile)
{
	// Read config file
	std::vector<Server> servers = readConfig(configFile);

	if (servers.empty())
		throw std::runtime_error("Error: no servers found in config file");

	// Add servers to listeners or create new listeners
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
		addServer(*it);

	// Set poll timeout
	FdHandler::setTimeout(POLL_TIMEOUT);
}

void Webserv::loop(void)
{
	// Poll
	FdHandler::poll();

	// Loop through listeners
	std::vector<Listener>::iterator it;
	for (it = _listeners.begin(); it != _listeners.end(); it++)
		it->loop();
}

std::ostream &operator<<(std::ostream &os, const Webserv &obj)
{
	os << "Listeners: " << obj._listeners.size() << std::endl;
	for (size_t i = 0; i < obj._listeners.size(); i++)
		os << i + 1 << ". " << obj._listeners[i];
	return (os);
}