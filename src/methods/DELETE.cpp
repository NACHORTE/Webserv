#include "HttpMethods.hpp"
#include "utils.hpp"
#include <fstream>
#include <cstdio>

HttpResponse DELETE(const HttpRequest & req, const Server & serv, const Location & loc)
{
	HttpResponse ret;

	std::string filename = loc.getFilename(req.get_path());

	// If a file was not found, return 404
	if (filename == "")
		return serv.errorResponse(404, "Not Found", "File not found");

	// check if file exists
	std::ifstream file(filename.c_str());
	if (!file.is_open())
		return serv.errorResponse(404, "Not Found", "File not found");
	file.close();

	// delete file
	if (std::remove(filename.c_str()) == 0)
		ret.setStatus(200, "OK");
	else
		return serv.errorResponse(500, "Internal Server Error", "Failed to delete file");

	return ret;
}
