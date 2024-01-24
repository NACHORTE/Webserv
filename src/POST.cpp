#include "HttpMethods.hpp"
#include "utils.hpp"

HttpResponse POST(const HttpRequest & req, const Locations & valid_paths)
{
	// NOTE ADD extension to uploaded files
	// CHECK for trying to create a file outside /upload
	/*	HttpResponse ret;
		std::string content_type = req["Content-Type"];
		std::string boundaryTag = "boundary=";
		size_t boundaryPos = content_type.find(boundaryTag);
		std::string boundary = content_type.substr(boundaryPos + boundaryTag.length());

		if (boundaryPos == std::string::npos)
		{
			std::cout << "No boundary found\n";
			ret.set_status(400, "Bad request");
			ret.set_body("text/html", "<html><body><h1>400 Bad request</h1></body></html>");
			return ret;
		}
		std::string boundaryDelimiter = "--" + msg.substr(boundaryPos + boundaryTag.length(), boundaryEndPos - (boundaryPos + boundaryTag.length()));
		std::cout << "\n\nBoundary MIO: " << boundaryDelimiter << "\n\n";
		std::string body = msg.substr(msg.find(boundaryDelimiter) + boundaryDelimiter.length() + 2);
		std::cout << "\n\nBody MIO: " << body << "\n\n";
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
