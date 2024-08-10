#include "Server.hpp"
#include "utils.hpp"
#include <iostream>
#include "dirent.h"
#include "unistd.h"
#include <sys/stat.h>
#include <set>

HttpResponse getAutoIndex(const std::string & path, const Location & loc, const Server & serv)
{
	std::string body = "<html><head><title>Index of " + path + "</title></head><body><h1>Index of " + path + "</h1><hr><pre>";

	std::string folder = loc.getFilenameNoIndex(path);
	if (folder.empty())
		return serv.errorResponse(404, "Not Found", "File not found");

	DIR *dir;
	struct dirent *ent;
	std::set<std::string> files;
	if ((dir = opendir(folder.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			std::string name = ent->d_name;
			if (name != ".")
				files.insert(name);
		}
		closedir(dir);
	}
	else
		body += "Could not open directory";

	for (std::set<std::string>::iterator it = files.begin(); it != files.end(); ++it)
	{
		body += "<a href=\"" + joinPath(path,*it) + "\">" + *it + "</a><br>";
	}
	body += "</pre><hr></body></html>";

	HttpResponse ret;
	ret.setStatus(200);
	ret.setBody("text/html",body);
	ret.responseReady(true);
	return ret;
}

void  Server::GET(MyPoll &myPoll, Client & client, const Location & loc)
{
	HttpResponse ret;

	std::string path = client.getRequest().getPath();
	// Get the path of the requested file
	std::string filename = loc.getFilename(path);
	if (filename.empty() or access(filename.c_str(), F_OK) != 0)
		return (void)client.setResponse(errorResponse(404, "Not Found", "File not found"));

	if (loc.isCgi())
		return (void)_activeCGI.newCgi(myPoll, client, filename, *this);

	// If the filename doesn't exist, check if it's a directory
	if (access(filename.c_str(),F_OK) != 0 and loc.isDir(path))
	{
		if (loc.autoIndex())
			return (void)client.setResponse(getAutoIndex(path, loc, *this));
		client.setResponse(errorResponse(403, "Forbidden", "Path is for a folder and location has no autoindex"));
		return;
		
	}

	// Get file information using stat
	struct stat fileInfo;
	if (stat(filename.c_str(), &fileInfo) != 0)
		return (void)client.setResponse(errorResponse(500, "Internal Server Error", "Could not get file information"));

	// Check if the file is a regular file
	if (!S_ISREG(fileInfo.st_mode))
		return (void)client.setResponse(errorResponse(403, "Forbidden", "Path is not a regular file"));

	try
	{
		ret.setStatus(200);
		ret.setBodyFromFile(filename);
		ret.responseReady(true);
		client.setResponse(ret);
	}
	catch (std::exception & e)
	{	
		client.setResponse(errorResponse(404, "Not Found", e.what()));
	}
}
