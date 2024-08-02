#include "Server.hpp"
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

void  Server::GET(Client & client, const Location & loc)
{
	HttpResponse ret;

	std::string path = client.getRequest().getPath();
	// Get the path of the requested file
	std::string filename = loc.getFilename(path);
	if (filename.empty())
		return (void)client.setResponse(errorResponse(404, "Not Found", "File not found"));

    if (loc.isCgi())
		return (void)_activeCGI.newCgi(client, filename, *this);

	try
	{
		ret.setStatus(200);
		ret.setBodyFromFile(filename);
	}
	catch (std::exception & e)
	{	
		// If the path is a directory and autoindex is enabled, return the autoindex
		if (loc.isDir(path))
		{
			if (loc.autoIndex())
				return (void)client.setResponse(getAutoIndex(path, loc, *this));
			client.setResponse(errorResponse(403, "Forbidden", "Path is for a folder and location has no autoindex"));
            return;
		}
		client.setResponse(errorResponse(404, "Not Found", e.what()));
        return;
	}
}
