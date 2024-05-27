#include "HttpMethods.hpp"
#include "utils.hpp"
#include <iostream>

HttpResponse GET(const HttpRequest & req, const LocationContainer & valid_paths)
{
	HttpResponse ret;

	// Get the path without the query string
	std::string path = req.get_path();
	path = path.substr(0, path.find('?'));

	// Get the path of the requested file
	std::string filename = valid_paths.getFilename(path);

	// The request was succesful
	try
	{
		ret.setStatus(200);
		ret.setBody(filename);
	}
	// Catch FileNotFound and return 404 Not Found
	catch (std::exception & e)
	{
		return HttpResponse::error(404, "Not Found", e.what());
	}

	return ret;
}
