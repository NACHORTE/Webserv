#include "Server.hpp"
#include "utils.hpp"
#include <fstream>
#include <cstdio>

void Server::DELETE(Client & client, const Location & loc)
{
	if (loc.isDir(client.getRequest().getPath()))
		return errorResponse(client, 403, "Forbidden", "Cannot delete a directory");	
	std::string filename = loc.getFilename(client.getRequest().getPath());

	// If a file was not found, return 404
	if (filename == "")
		return errorResponse(client, 404, "Not Found", "File not found");

	// check if file exists
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		return errorResponse(client, 404, "Not Found", "File not found");
	file.close();

	// delete file
	if (std::remove(filename.c_str()) == 0)
		errorResponse(client, 200, "OK", "File deleted");
	else
		errorResponse(client, 500, "Internal Server Error", "Failed to delete file");
}
