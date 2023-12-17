#include "utils.hpp"
#include <sstream>
#include <fstream>

std::string int_to_string(int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

std::string readImageFile(const std::string& filePath) {
    std::ifstream imageFile(filePath, std::ios::binary);

    if (!imageFile) {
        std::cerr << "Error opening image file: " << filePath << std::endl;
        return "";
    }

    std::ostringstream imageContent;
    imageContent << imageFile.rdbuf();
    return imageContent.str();
}

std::string readHtmlFile(const std::string& filePath) {
    std::ifstream htmlFile(filePath);

    if (!htmlFile.is_open()) {
        if (htmlFile.fail()) {
            // Opening the file failed for a reason other than "file not found"
            std::cerr << "Error opening HTML file (other error): " << filePath << std::endl;
            throw std::runtime_error("Error opening HTML file (other error): " + filePath);
        } else {
            // The file was not found
            std::cerr << "File not found: " << filePath << std::endl;
            throw std::invalid_argument("File not found: " + filePath);
        }
    }

    std::ostringstream htmlContent;
    htmlContent << htmlFile.rdbuf();
    return htmlContent.str();
}
