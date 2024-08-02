#include "CGI.hpp"
#include "utils.hpp"
#include <fstream>
#include "Server.hpp"
#include "unistd.h"
#include "stdlib.h"
#include <fcntl.h>

static char **getPath(const std::string & filename)
{
	char **output = new char*[2];
	if (output == NULL)
		return NULL;
	output[0] = strdup(filename.c_str());
	output[1] = NULL;
    if (output[0] == NULL)
    {
        delete[] output;
        return NULL;
    }
	return output;
}

static char **getEnv(const HttpRequest & req)
{
	// Get the query string
	std::string queryString ="QUERY_STRING=" + req.getQueryString();

	// Get the method and uri
	std::string method = std::string("REQUEST_METHOD=") + req.getMethod();
	std::string uri = std::string("REQUEST_URI=") + req.getPath();
	if (method.substr(15).empty() || uri.substr(12).empty())
		return NULL;

	// Get the headers
	std::list<std::string> headerList;
	std::vector<std::pair<std::string,std::string> > headerVector = req.getHeaders();
	for (size_t i = 0; i < headerVector.size(); ++i)
		headerList.push_back(headerVector[i].first + "=" + headerVector[i].second);

	// Generate the environment variables
	size_t outputSize = headerList.size() + 4; // 3 for the query string, method and uri, 1 for the NULL at the end
	char **output = new char*[outputSize];
	if (output == NULL)
		return NULL;
	output[0] = strdup(queryString.c_str());
	output[1] = strdup(method.c_str());
	output[2] = strdup(uri.c_str());
	size_t i = 3;
	for (std::list<std::string>::iterator it = headerList.begin(); it != headerList.end(); ++it)
		output[i++] = strdup(*it);
	output[i] = NULL;

	// Check if any strdup failed
	for (size_t i = 0; i + 1 < outputSize; ++i)
	{
		if (output[i] == NULL)
		{
			for (size_t j = 0; j < outputSize; ++j)
				if (output[j] != NULL)
					delete[] output[j];
			delete[] output;
			return NULL;
		}
	}

	return output;
}

void CGI::newCgi(Client &client, const std::string & filename, const Server& server)
{
	// Check if the client is already waiting for a CGI program to finish
    for (size_t i = 0; i < _clients.size(); ++i)
	{
        if (_clients[i]._client == &client)
            return;
	}

	// Check if the file exists and can be executed
	std::ifstream file(filename.c_str());
	if (!file.good())
		return (void)client.setResponse(server.errorResponse(404, "not_found", "file not found"));
	if (access(filename.c_str(), X_OK) != 0)
		return (void)client.setResponse(server.errorResponse(403, "forbidden", "file not executable"));
	file.close();

	// Create a pipe to communicate with the CGI program
	int fdsIn[2], fdsOut[2];
	if (pipe(fdsIn) != 0 || pipe(fdsOut) != 0)
		return (void)client.setResponse(server.errorResponse(500, "internal_server_error", "pipe failed"));

	// Fork the process
	int pid = fork();
	if (pid == -1)
	{
		close(fdsIn[0]);
		close(fdsIn[1]);
		close(fdsOut[0]);
		close(fdsOut[1]);
		return (void)client.setResponse(server.errorResponse(500, "internal_server_error", "fork failed"));
	}
	// Child process
	if (pid == 0)
	{
		// Redirect stdout of the fork program to the pipe
		dup2(fdsIn[1],STDOUT_FILENO);
		close(fdsIn[1]);
		close(fdsIn[0]);
		// Redirect stdin of the CGI program to the pipe
		dup2(fdsOut[0],STDIN_FILENO);
		close(fdsOut[1]);
		close(fdsOut[0]);
		// Get the path and environment variables for the CGI program
		char **args = getPath(filename);
		char **envp = getEnv(client.getRequest());
		if (args == NULL || envp == NULL)
			exit(EXIT_FAILURE);
		// Execute the CGI program
		execve(args[0], args, envp);
		exit(EXIT_FAILURE);
	}
	// Parent process
	// Add the client to the list of clients waiting for the CGI program to finish
	_clients.push_back(CgiClient(client, pid, fdsOut[1], fdsIn[0]));
	close(fdsIn[1]);
	close(fdsOut[0]);
	fcntl(fdsIn[0], F_SETFL, O_NONBLOCK);
	fcntl(fdsOut[1], F_SETFL, O_NONBLOCK);

    // Add the pipes to the pollfd list
    pollfd pollfdIn;
    pollfdIn.fd = fdsIn[0];
    pollfdIn.events = POLLIN | POLLHUP | POLLERR;
    _pollfdIn.push_back(pollfdIn);
    pollfd pollfdOut;
    pollfdOut.fd = fdsOut[1];
    pollfdOut.events = POLLOUT | POLLHUP | POLLERR;
    _pollfdOut.push_back(pollfdOut);
}