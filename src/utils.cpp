#include "utils.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

std::string int_to_string(int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

std::string readImageFile(const std::string& filePath) {
    std::ifstream imageFile(filePath.c_str(), std::ios::binary);

    if (!imageFile) {
        std::cerr << "Error opening image file: " << filePath << std::endl;
        return "";
    }

    std::ostringstream imageContent;
    imageContent << imageFile.rdbuf();
    return imageContent.str();
}

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath.c_str());

    if (!file.is_open()) {
        if (file.fail()) {
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
    htmlContent << file.rdbuf();
    return htmlContent.str();
}

int ok_config(std::string config_file)
{
	if (config_file.rfind(".conf") != config_file.length() - 5)
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
