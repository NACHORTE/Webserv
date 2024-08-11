#include "Server.hpp"
#include "utils.hpp"
#include <iostream>
#include "dirent.h"
#include "unistd.h"
#include <sys/stat.h>
#include <set>
#include <fcntl.h>
#include "FdHandler.hpp"

static void getAutoIndex(Client &client, const std::string & path, const Location & loc, Server & serv)
{
	std::string body = "<html><head><title>Index of " + path + "</title></head><body><h1>Index of " + path + "</h1><hr><pre>";

	std::string folder = loc.getFilenameNoIndex(path);
	if (folder.empty())
		return serv.errorResponse(client, 404, "Not Found", "File not found");

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
	ret.setHeader("Content-Type", "text/html");
	ret.setBody(body);
	ret.responseReady(true);
	client.getResponse() = ret;
}

void  Server::GET(Client & client, const Location & loc)
{
	std::string path = client.getRequest().getPath();
	// Get the path of the requested file
	std::string filename = loc.getFilename(path);
	if (filename.empty() or access(filename.c_str(), F_OK) != 0)
		return errorResponse(client, 404, "Not Found", "File not found");

	if (loc.isCgi())
		return (void)_activeCGI.newCgi(client, filename, *this);

	// If the filename doesn't exist, check if it's a directory
	if (access(filename.c_str(),F_OK) != 0 and loc.isDir(path))
	{
		if (loc.autoIndex())
			return getAutoIndex(client, path, loc, *this);
		return errorResponse(client, 403, "Forbidden", "Path is for a folder and location has no autoindex");
	}

	// Get file information using stat
	struct stat fileInfo;
	if (stat(filename.c_str(), &fileInfo) != 0)
		return errorResponse(client, 500, "Internal Server Error", "Could not get file information");

	// Check if the file is a regular file
	if (!S_ISREG(fileInfo.st_mode))
		return errorResponse(client, 403, "Forbidden", "Path is not a regular file");

	try
	{
		// Open the file
		int fd = open(filename.c_str(), O_RDONLY);
		if (fd == -1)
			throw std::exception();
		if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		{
			close(fd);
			return errorResponse(client, 500, "Internal Server Error", "Could not set file descriptor to non-blocking");
		}
		client.getResponse().clear();
		client.getResponse().setStatus(200, "OK");
		client.getResponse().setHeader("Content-Type", extToMime(filename));

		_staticResponses[&client] = std::make_pair(fd, "");
		FdHandler::addFd(fd, POLLIN);
	}
	catch (std::exception & e)
	{	
		errorResponse(client, 404, "Not Found", e.what());
	}
}
