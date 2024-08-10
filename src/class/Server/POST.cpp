#include "Server.hpp"
#include "utils.hpp"
#include <map>
#include <vector>
#include <fstream>

void Server::POST(MyPoll &myPoll, Client & client, const Location & loc)
{
	std::string filename = loc.getFilename(client.getRequest().getPath());
	if (filename.empty())
		return (void)client.setResponse(errorResponse(404, "Not Found", "File not found"));

	_activeCGI.newCgi(myPoll, client, filename, *this);
}
