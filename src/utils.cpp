#include "utils.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

std::string int_to_string(int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

std::string readFile(const std::string& filePath, bool binary = false) {
	// Open the file in binary or text mode
    std::ifstream file(filePath.c_str(), binary ? std::ios::binary : std::ios::in);

	// If file wasn't opened, throw an exception
    if (!file.is_open())
	{
		// Opening the file failed for a reason other than "file not found"
		if (file.fail())
		{
		    std::cerr << "Error opening HTML file (other error): " << filePath << std::endl; //NOTE remove this message maybe
		    throw FileNotOpen();
		}
		// The file was not found
		std::cerr << "File not found: " << filePath << std::endl; //NOTE remove this message maybe
		throw FileNotFound();
    }

	// Read the file into a string
    std::ostringstream htmlContent;
    htmlContent << file.rdbuf();
    return htmlContent.str();
	// The file closes automatically when it goes out of scope
}

int ok_config(std::string config_file)
{
	//if (config_file.rfind(".conf") != config_file.length() - 5)
	if (getExtension(config_file) != "conf")
	{
        std::cerr << "Config file does not have .conf extension: " << config_file << std::endl;
        return 1;
    }
	std::ifstream file(config_file.c_str());
	if(!file.is_open())
	{
		std::cerr << "Error opening config file: " << config_file << std::endl;
		return 1;
	}
	return 0;
}

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first)
        return str;
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

char back(std::string str)
{
	return str[str.length() - 1];
}

void pop_back(std::string& str)
{
	str.erase(str.length() - 1);
}

std::string getExtension(const std::string & filename)
{
	// Get the position of the last '.' in the filename
	size_t pos = filename.find_last_of('.');

	// If there is no '.' or file ends with '.', return an empty string
	if (pos == std::string::npos || pos == filename.length() - 1)
		return "";
	
	// Return the extension (everything after the last '.' in the filename)
	return filename.substr(pos + 1);
}

std::string getContentType(const std::string & path) {
	// List of extension-content_type pairs that only initializes once
	static std::map<std::string, std::string> content_type;
	if (content_type.empty())
	{
		content_type["html"] = "text/html";
		content_type["css"] = "text/css";
		content_type["js"] = "application/javascript";
		content_type["jpg"] = "image/jpeg";
		content_type["jpeg"] = "image/jpeg";
		content_type["png"] = "image/png";
		content_type["gif"] = "image/gif";
		content_type["swf"] = "application/x-shockwave-flash";
		content_type["txt"] = "text/plain";
		content_type["pdf"] = "application/pdf";
		content_type["mp3"] = "audio/mpeg";
		content_type["mp4"] = "video/mp4";
		content_type["avi"] = "video/x-msvideo";
		content_type["mpeg"] = "video/mpeg";
		content_type["ico"] = "image/x-icon";
	}

	// Get the extension of the file
	std::string extension = getExtension(path);

	// If the extension is not in the map, return "application/octet-stream"
	if (extension == "" || content_type.count(extension) == 0)
		return "application/octet-stream";
	
	// Return the content type
	return content_type[extension];
}
