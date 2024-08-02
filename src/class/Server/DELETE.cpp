#include "Server.hpp"
#include "utils.hpp"
#include <fstream>
#include <cstdio>

void Server::DELETE(Client & client, const Location & loc)
{

	std::string filename = loc.getFilename(client.getRequest().getPath());

	// If a file was not found, return 404
	if (filename == "")
		return (void)client.setResponse(errorResponse(404, "Not Found", "File not found"));

	// check if file exists
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		return (void)client.setResponse(errorResponse(404, "Not Found", "File not found"));
	file.close();

	// delete file
	if (std::remove(filename.c_str()) == 0)
		client.setResponse(errorResponse(200, "OK", "File deleted"));
	else
		client.setResponse(errorResponse(500, "Internal Server Error", "Failed to delete file"));
}
