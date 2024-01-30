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
		ret.set_status(404, "Not Found");
		ret.set_body("text/html", "<html><body><h1>404 Not Found</h1></body></html>");
		return ret;
	}

	// check if file exists
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		ret.set_status(404, "Not Found");
		ret.set_body("text/html", "<html><body><h1>404 Not Found</h1></body></html>");
		return ret;
	}
	file.close();

	// delete file
	if (std::remove(filename.c_str()) == 0)
		ret.set_status(200, "OK");
	else
	{
		ret.set_status(500, "Internal Server Error");
		ret.set_body("text/html", "<html><body><h1>200 OK</h1></body></html>");
	}
	return ret;
}
