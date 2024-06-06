#include "HttpMethods.hpp"
#include "utils.hpp"
#include <iostream>

HttpResponse GET(const HttpRequest & req, const Server & serv, const Location & loc)
{
	HttpResponse ret;

	// Get the path without the query string
	std::string path = req.get_path();
	path = cleanPath(decodeURL(path.substr(0, path.find('?'))));

	// Get the path of the requested file
	std::string filename = loc.getFilename(path);
	if (filename.empty())
		return serv.errorResponse(404, "Not Found", "File not found");

	// The request was succesful
	try
	{
		ret.setStatus(200);
		ret.setBody(filename);
	}
	// Catch FileNotFound and return 404 Not Found
	catch (std::exception & e)
	{
		return serv.errorResponse(404, "Not Found", e.what());
	}

	return ret;
}
