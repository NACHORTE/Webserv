#include "Server.hpp"
#include "utils.hpp"
#include <map>
#include <vector>
#include <fstream>

void Server::POST(Client & client, const Location & loc)
{
	std::string filename = loc.getFilename(client.getRequest().getPath());
	if (filename.empty())
		return errorResponse(client, 404, "Not Found", "File not found");

	_activeCGI.newCgi(client, filename, *this);
}
