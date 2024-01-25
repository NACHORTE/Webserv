#include "HttpMethods.hpp"
#include "utils.hpp"
#include <map>

HttpResponse POST(const HttpRequest & req, const Locations & valid_paths)
{
	// TODO ADD extension to uploaded files
	// NOTE only /upload allows to create a file
	/*HttpResponse ret;

	// Get the content of the content-type header
	std::vector<std::string> content_type_header = req["Content-Type"];
	if (content_type_header.size() != 1)
		return (ret.set_status(400, "Bad request"),
			ret.set_body("text/html", "<html><body><h1>400 Bad request</h1><p>incorrent amount of content-type</p></body></html>"),
			ret);
	// Get the content-type name
	std::string content_type_name = content_type_header[0].substr(0, content_type_header[0].find(";"));
	std::map<std::string,std::string> content_type_params = get_params(content_type_header[0]);
	// TODO Check if the content-type is multipart/form-data
	// Get the boundary
	std::string boundaryTag = "boundary=";
	size_t boundaryPos = content_type.find(boundaryTag);
	if (boundaryPos == std::string::npos)
		return(ret.set_status(400, "Bad request"),
			ret.set_body("text/html", "<html><body><h1>400 Bad request</h1><p></p></body></html>"),
			ret);
	std::string boundary = content_type.substr(boundaryPos + boundaryTag.length());
	// If the boundary is empty, return 400 Bad Request
	if (boundary.length() == 0)
		return(ret.set_status(400, "Bad request"),
			ret.set_body("text/html", "<html><body><h1>400 Bad request</h1><p></p></body></html>"),
			ret);

	std::string boundaryDelimiter = "--" + boundary;

		std::string body = msg.substr(msg.find(boundaryDelimiter) + boundaryDelimiter.length() + 2);
		std::string filename;
		std::string contentType;
		size_t filenameStart = body.find("filename=\"");
		size_t filenameEnd = body.find("\"", filenameStart + 10);
		if (filenameStart != std::string::npos && filenameEnd != std::string::npos) {
			filename = body.substr(filenameStart + 10, filenameEnd - (filenameStart + 10));
			std::cout << "Nombre del archivo: " << filename << std::endl;
		}
		// Buscar el tipo de contenido
		size_t contentTypeStart = body.find("Content-Type: ");
		size_t contentTypeEnd = body.find("\n", contentTypeStart);
		if (contentTypeStart != std::string::npos && contentTypeEnd != std::string::npos) {
			contentType = body.substr(contentTypeStart + 14, contentTypeEnd - (contentTypeStart + 14));
			std::cout << "Tipo de contenido: " << contentType << std::endl;
		}
		size_t binaryDataStart = body.find("\r\n\r\n");
		std::string binaryData;
		if (binaryDataStart != std::string::npos) 
		{
			binaryData = body.substr(binaryDataStart + 4);
		}
		std::cout << "Binary data: " << binaryData << std::endl;
		std::cout << "Hexadecimal representation of binary data:\n";
		for (char c : binaryData) {
			printf("%02X ", static_cast<unsigned char>(c));
		}
		std::cout << "\n";
		std::ofstream outputFile("./upload/" + filename, std::ios::binary);
		outputFile.write(binaryData.c_str(), binaryData.size());
		outputFile.close();
		n = write(connfd, htmlUpload.to_string().c_str(), htmlUpload.to_string().length());*/
	(void)req;
	(void)valid_paths;
	HttpResponse ret;
	ret.set_status(200, "POST OK");
	ret.set_body("text/html", "<html><body><h1>POST</h1></body></html>");
	return ret;
}
