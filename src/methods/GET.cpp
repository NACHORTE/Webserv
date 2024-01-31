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
		return HttpResponse::error(501, "Not Implemented", "CGI is not implemented yet");
	}

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
