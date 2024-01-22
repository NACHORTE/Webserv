#include "HttpMethods.hpp"
#include "utils.hpp"

HttpResponse GET(const HttpRequest & req, const Locations & valid_paths)
{
	HttpResponse ret;

	// Get the path of the requested file
	std::string filename = valid_paths.getFilename(req.get_path());

	// If the file ends with ".cgi", run the file and return the output
	if (getExtension(filename) == "cgi")
	{
		//something with fork and execve
		ret.set_status(200, "CGI OK");
		ret.set_body("text/html", "<html><body><h1>CGI</h1></body></html>");
		return ret;
	}

	// The request was succesful
	try
	{
		ret.set_status(200);
		ret.set_body(getContentType(filename), readFile(filename, isBinaryFile(filename)));
	}
	// Catch FileNotFound and return 404 Not Found
	catch (std::exception & e)
	{
		// NOTE Maybe try a file and if it fails, return this
		ret.set_status(404);
		ret.set_body("text/html", "<html><body><h1>404 Not Found</h1></body></html>");
	}

	return ret;
}
