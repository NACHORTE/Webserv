#include "HttpMethods.hpp"
#include "utils.hpp"
#include <iostream>
#include "dirent.h"

HttpResponse getAutoIndex(const std::string & path, const Location & loc, const Server & serv)
{
	std::string body = "<html><head><title>Index of " + path + "</title></head><body><h1>Index of " + path + "</h1><hr><pre>";

	std::string folder = loc.getFilenameNoIndex(path);
	if (folder.empty())
		return serv.errorResponse(404, "Not Found", "File not found");

	DIR *dir;
	struct dirent *ent;
	std::vector<std::string> files;
	if ((dir = opendir(folder.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			std::string name = ent->d_name;
			if (name != "." && name != "..")
				files.push_back(name);
		}
		closedir(dir);
	}
	else
		body += "Could not open directory";

	for (size_t i = 0; i < files.size(); ++i)
		body += "<a href=\"" + path + "/" + files[i] + "\">" + files[i] + "</a><br>";
	body += "</pre><hr></body></html>";

	HttpResponse ret;
	ret.setStatus(200);
	ret.setBody("text/html",body);
	return ret;
}

HttpResponse GET(const HttpRequest & req, const Server & serv, const Location & loc)
{
	HttpResponse ret;

	// Get the path without the query string
	std::string path = req.getPath();
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
		if (loc.isDir(path) && loc.autoIndex())
			return getAutoIndex(path, loc, serv);
		return serv.errorResponse(404, "Not Found", e.what());
	}

	return ret;
}
