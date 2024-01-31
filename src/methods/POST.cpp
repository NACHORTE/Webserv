#include "HttpMethods.hpp"
#include "utils.hpp"
#include <map>
#include <vector>
#include <fstream>

#define CONTENT_DISPOSITION "Content-Disposition"
#define CONTENT_TYPE "Content-Type"

struct MultipartForm
{
    std::string name;
    std::string filename;
    std::string contentType;
    std::string data;
};

// Function for analyzing the Content-Type header
static std::vector<MultipartForm> parseMultipartForm(const std::string& body, std::string boundary)
{
    std::vector<MultipartForm> parts;

	boundary = "--" + boundary;
	size_t bLen = boundary.length();

	// Loop through the parts
    size_t pos = 0;
    while ((pos = body.find(boundary, pos)) != std::string::npos // Find the next part
		&& body.substr(pos + bLen, 2) != "--")	// Exit the loop if this part starts with --boundary--
	{
		MultipartForm form;

		// Advance begin to skip the boundary
        pos += bLen;

		// Get the part substring
		std::string part = body.substr(pos, body.find(boundary, pos) - pos);

		// Get headers using header separator '\n' and key-value separator ':'
		std::map<std::string, std::string> headers = get_params(part.substr(0, part.find("\r\n\r\n")), '\n', ':');
		for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
		// If there is no Content-Disposition header, skip this part
		if (headers.count(CONTENT_DISPOSITION) == 0)
			continue;
		// If there is no Content-Type header, set it to application/octet-stream
		if (headers.count(CONTENT_TYPE) == 0)
			headers[CONTENT_TYPE] = "application/octet-stream";

		// get Content-Disposition and its parameters
		std::string contentDisposition = headers[CONTENT_DISPOSITION].substr(0, headers[CONTENT_DISPOSITION].find(";"));
		std::map<std::string, std::string> CDParams = get_params(headers[CONTENT_DISPOSITION], ';', '=');
		// If there is no name parameter, skip this part
		if (CDParams.count("name") == 0 || CDParams["name"].length() == 0)
			continue;
		CDParams["name"] = removeQuotes(CDParams["name"]);
		// Set the filename parameter to the name parameter if it doesn't exist
		if (CDParams.count("filename") == 0 || CDParams["filename"].length() == 0)
			CDParams["filename"] = CDParams.count("name");
		else
			CDParams["filename"] = removeQuotes(CDParams["filename"]);
		// Save everything to the form
		form.name = CDParams["name"];
		form.filename = CDParams["filename"];
		form.contentType = headers[CONTENT_TYPE];
		form.data = part.substr(part.find("\r\n\r\n") + 4);

		// Add the form to the vector
		parts.push_back(form);
    }
	for (std::vector<MultipartForm>::iterator it = parts.begin(); it != parts.end(); it++)
	{
		std::cout << "name: " << it->name << std::endl;
		std::cout << "filename: " << it->filename << std::endl;
		std::cout << "contentType: " << it->contentType << std::endl;
		if (it->data.size() < 100)
			std::cout << "data: " << it->data << std::endl;
		else
			std::cout << "data: " << it->data.substr(0,100) << "..." << std::endl;
	}
	return parts;
}

HttpResponse POST(const HttpRequest & req, const Locations & valid_paths)
{
	// TODO ADD extension to uploaded files
	// TODO manage upload file size
	// NOTE only /upload allows to create a file
	HttpResponse ret;

	// Get the content of the content-type header
	std::vector<std::string> contentTypeHeader = req["Content-Type"];
	if (contentTypeHeader.size() != 1)
		return HttpResponse::error(400, "Bad Request", "Content-Type header must be specified once");

	// Get the content-type name and parameters
	std::string contentTypeName = contentTypeHeader[0].substr(0, contentTypeHeader[0].find(";"));
	std::map<std::string,std::string> contentTypeParams = get_params(contentTypeHeader[0]);

	// If there is a charset specified and it's not UTF-8, return 415 Unsupported Media Type
	if (contentTypeParams.count("charset") == 1 && contentTypeParams["charset"] != "UTF-8")
		return HttpResponse::error(415, "Unsupported Media Type", "Only UTF-8 charset is supported");

	// If the content type is multipart/form-data, parse the body
	if (contentTypeName == "multipart/form-data")
	{
		// If there is no boundary specified, return 400 Bad Request
		if ((contentTypeParams.count("boundary") == 0
			|| contentTypeParams["boundary"].length() == 0))
			return HttpResponse::error(400, "Bad Request", "multipart/form-data requires a boundary");

		std::vector<MultipartForm> forms = parseMultipartForm(req.getBody(), contentTypeParams["boundary"]);
		//NOTE ADD HERE THE EXTENSION
		size_t len = forms.size();
		for (size_t i = 0; i < len; i++)
		{
			std::string filename = valid_paths.getFilename(req.get_path()) + "/" + forms[i].filename;
			std::ofstream file(filename.c_str());
			file << forms[i].data;
			file.close();
		}
	}
	ret.setStatus(200, "POST OK");
	ret.setBody("text/html", "<html><body><h1>POST</h1></body></html>");
	return ret;
}
