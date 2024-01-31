#include "HttpMethods.hpp"
#include "utils.hpp"

HttpResponse GET(const HttpRequest & req, const Locations & valid_paths)
{
	HttpResponse ret;

	// Get the path without the query string
	std::string path = req.get_path();
	path = path.substr(0, path.find('?'));

	// Get the path of the requested file
	std::string filename = valid_paths.getFilename(path);

	// If the file ends with ".cgi", run the file and return the output
	if (getExtension(path) == "cgi")
	{
		//char **envp = getEnvp(req.get_path());
		//something with fork and execve
		ret.setStatus(200, "CGI OK");
		ret.setBody("text/html", "<html><body><h1>CGI</h1></body></html>");
		return ret;
	}

	// The request was succesful
	try
	{
		ret.setStatus(200);
		ret.setBody(extToMime(filename), readFile(filename, isBinaryFile(filename)));
	}
	// Catch FileNotFound and return 404 Not Found
	catch (std::exception & e)
	{
		// NOTE Maybe try a file and if it fails, return this
		ret.setStatus(404);
		ret.setBody("text/html", "<html><body><h1>404 Not Found</h1></body></html>");
	}

	return ret;
}
