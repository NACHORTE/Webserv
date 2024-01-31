#include "HttpMethods.hpp"
#include "utils.hpp"
#include <fstream>
#include <cstdio>

HttpResponse DELETE(const HttpRequest & req, const Locations & valid_paths)
{
	HttpResponse ret;

	std::string filename = valid_paths.getFilename(req.get_path());

	// If a file was not found, return 404
	if (filename == "")
	{
		ret.setStatus(404, "Not Found");
		ret.setBody("text/html", "<html><body><h1>404 Not Found</h1></body></html>");
		return ret;
	}

	// check if file exists
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		ret.setStatus(404, "Not Found");
		ret.setBody("text/html", "<html><body><h1>404 Not Found</h1></body></html>");
		return ret;
	}
	file.close();

	// delete file
	if (std::remove(filename.c_str()) == 0)
		ret.setStatus(200, "OK");
	else
	{
		ret.setStatus(500, "Internal Server Error");
		ret.setBody("text/html", "<html><body><h1>200 OK</h1></body></html>");
	}
	return ret;
}
