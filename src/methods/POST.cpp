#include "HttpMethods.hpp"
#include "utils.hpp"
#include <map>
#include <vector>
#include <fstream>
#include "colors.h"

#define CONTENT_DISPOSITION "Content-Disposition"
#define CONTENT_TYPE "Content-Type"
#define NAME "name"
#define FILENAME "filename"

/**
 * @brief A structure representing a part of a multipart/form-data request.
 *
 * This structure contains the name of the part, the filename (if applicable),
 * the content type (if applicable) and the data of the part.
 *
 * @see parseMultipartForm
 */
struct MultipartForm
{
    std::string name;
    std::string filename;
    std::string contentType;
    std::string data;
};

/**
 * @brief Parse the body of a multipart/form-data request.
 *
 * This function extracts information from the body of a multipart/form-data
 * request, which may contain multiple parts including file uploads.
 *
 * @param body The body of the HTTP request containing multipart data.
 * @param boundary The boundary string used to separate parts in the body.
 *
 * @return A vector of MultipartForm structures, each representing a part of
 *         the multipart form. If parsing fails or no valid parts are found,
 *         the vector will be empty.
 *
 * @note The function expects the "Content-Disposition" header to be present in
 *       each part. Parts without this header will be skipped.
 *
 * @see MultipartForm
 * @see get_params
 * @see removeQuotes
 */
static std::vector<MultipartForm> parseMultipartForm(const std::string& body, std::string boundary)
{
    std::vector<MultipartForm> ret;

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
		// If there is no Content-Disposition header, skip this part
		if (headers.count(CONTENT_DISPOSITION) == 0)
			continue;

		// get Content-Disposition and its parameters
		std::string contentDisposition = headers[CONTENT_DISPOSITION].substr(0, headers[CONTENT_DISPOSITION].find(";"));
		std::map<std::string, std::string> CDParams = get_params(headers[CONTENT_DISPOSITION], ';', '=');
		// If there is no name parameter, skip this part
		if (CDParams.count(NAME) == 0 || CDParams[NAME].length() == 0)
			continue;
		// Remove quotes from name and filename (if filename doesn't exist, it will be empty)
		CDParams[NAME] = removeQuotes(CDParams[NAME]);
		CDParams[FILENAME] = removeQuotes(CDParams[FILENAME]);

		// Save everything to the form and push it to the vector
		form.name = CDParams[NAME];
		form.filename = CDParams[FILENAME];
		form.contentType = headers[CONTENT_TYPE];
		form.data = part.substr(part.find("\r\n\r\n") + 4);
		ret.push_back(form);
    }
	return ret;
}

/**
 * @brief Create a file from a MultipartForm structure.
 *
 * This function creates a file from a MultipartForm structure. If the file
 * already exists, it will be overwritten.
 *
 * @param form The MultipartForm structure containing the data to write to the
 *             file.
 * @param req The HttpRequest object containing the request data.
 * @param valid_paths The Locations object containing the valid paths.
 *
 * @return 0 on success, -1 on failure.
 *
 * @see MultipartForm
 * @see HttpRequest
 * @see Locations
 */
static int createFile(const MultipartForm & form, const HttpRequest & req, const Locations & valid_paths)
{
	// If the filename contains .., return -1
	if (form.filename.find("..") != std::string::npos)
		return -1;

	// Get the path of the file and open it
	std::string filename = valid_paths.getFilename(req.get_path()) + "/" + form.filename;
	std::ofstream file(filename.c_str());
	
	// If the file couldn't be opened, return -1
	if (!file.is_open())
		return -1;

	// Add the data to the file
	file << form.data;

	return 0;
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

		// 
		std::vector<MultipartForm> forms = parseMultipartForm(req.getBody(), contentTypeParams["boundary"]);
		std::vector<MultipartForm>::iterator form;
		for (form = forms.begin(); form != forms.end(); form++)
		{
			// If there is a filename, create a file and write the data to it
			if (!form->filename.empty())
			{
				if (createFile(*form, req, valid_paths) != 0)
					return HttpResponse::error(500, "Internal Server Error", "Failed to create file" + form->filename);
			}
			else
			{
				std::cout << YELLOW;
				std::cout << "Received form: " << form->name << std::endl;
				std::cout << "Filename: " << form->filename << std::endl;
				std::cout << "Content-Type: " << form->contentType << std::endl;
				std::cout << "Data: " << form->data << std::endl;
				std::cout << RESET;
			}
		}
	}
	ret.setStatus(200, "POST OK");
	ret.setBody("text/html", "<html><body><h1>POST</h1></body></html>");
	return ret;
}
