#include "Webserv.hpp"

Webserv::Webserv(void)
{}

Webserv::~Webserv()
{
	// Close all file descriptors
	for (std::vector<Listener>::iterator it = _listeners.begin(); it != _listeners.end(); it++)
		it->closeFds();
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

std::vector<Server> Webserv::read_config(const std::string& config_file) //XXX for test only
{
	(void)config_file;
	std::vector<Server> servers;
	Server server;
	server.setPort(8080);
	server.setRoot("/var/www/html");
	server.addServerName("localhost");
	server.addServerName("feo1");
	servers.push_back(server);
 	server= Server();
	server.setPort(8080);
	server.setRoot("/var/www/html");
	server.addServerName("server2");
	servers.push_back(server); 
 	server= Server();
	server.setPort(8081);
	server.setRoot("/var/www/html");
	server.addServerName("server3");
	server.addServerName("server3_1");
	server.addServerName("server3_2");
	servers.push_back(server);

	return servers;
}

std::ostream &operator<<(std::ostream &os, const Webserv &obj)
{
	os << "Listeners: " << obj._listeners.size() << std::endl;
	for (size_t i = 0; i < obj._listeners.size(); i++)
		os << i + 1 << ". =========\n" << obj._listeners[i];
	return (os);
}