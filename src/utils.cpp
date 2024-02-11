#include "utils.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <cctype>

static std::map<std::string, std::string> _ext_mime_map;

static void initext_mime_map()
{
	_ext_mime_map["txt"] = "text/plain";
	_ext_mime_map["htm"] = "text/html";
	_ext_mime_map["html"] = "text/html";
	_ext_mime_map["css"] = "text/css";
	_ext_mime_map["js"] = "application/javascript";
	_ext_mime_map["jpg"] = "image/jpeg";
	_ext_mime_map["jpeg"] = "image/jpeg";
	_ext_mime_map["png"] = "image/png";
	_ext_mime_map["gif"] = "image/gif";
	_ext_mime_map["swf"] = "application/x-shockwave-flash";
	_ext_mime_map["txt"] = "text/plain";
	_ext_mime_map["pdf"] = "application/pdf";
	_ext_mime_map["mp3"] = "audio/mpeg";
	_ext_mime_map["mp4"] = "video/mp4";
	_ext_mime_map["avi"] = "video/x-msvideo";
	_ext_mime_map["mpeg"] = "video/mpeg";
	_ext_mime_map["ico"] = "image/x-icon";
	_ext_mime_map["bin"] = "application/octet-stream";
}

/**
 * @brief Converts an integer to a string.
 * 
 * This function takes an integer as input and returns a string representing that number.
 * 
 * @param n Integer to convert to a string.
 * @return A string representing the integer.
 * 
 * @example
 * int number = 42;
 * std::string result = int_to_string(number);
 * // Expected result: "42"
 */
std::string int_to_string(int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

/**
 * @brief Reads the contents of a file and returns them as a string.
 * 
 * This function opens a file specified by the given file path and reads its contents into a string.
 * 
 * @param filePath Path to the file to be read.
 * @param binary Indicates whether to open the file in binary mode.
 * @return A string containing the contents of the file.
 * 
 * @throws std::runtime_error If the file cannot be opened.
 * 
 * @example
 * std::string filePath = "example.txt";
 * std::string fileContent = readFile(filePath, false);
 */
std::string readFile(const std::string& filePath, bool binary)
{
	// Open the file in binary or text mode
    std::ifstream file(filePath.c_str(), binary ? std::ios::binary : std::ios::in);

	// If file wasn't opened, throw an exception
    if (!file.is_open())
		throw std::runtime_error("Could not open file: " + filePath);

	// Read the file into a string
    std::ostringstream htmlContent;
    htmlContent << file.rdbuf();

    return htmlContent.str();
}

// XXX FUERA DE AQUI
int ok_config(std::string config_file)
{
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

/**
 * @brief Trims specified characters from the beginning and end of a string.
 * 
 * This function removes characters specified in the given charset from the beginning and end of the input string.
 * If no charset is provided, it trims whitespace characters by default.
 * 
 * @param str The input string to be trimmed.
 * @param charset The characters to be trimmed from the string (default is whitespace).
 * @return A string with specified characters removed from the beginning and end.
 * 
 * @example
 * std::string input = "   example   ";
 * std::string trimmed = trim(input);
 * // Result: "example"
 */
std::string trim(const std::string& str, const std::string & charset)
{
    size_t first = str.find_first_not_of(charset);
    if (std::string::npos == first)
        return str;
    size_t last = str.find_last_not_of(charset);
    return str.substr(first, (last - first + 1));
}

/**
 * @brief Returns the last character of a string.
 * 
 * This function takes a string as input and returns its last character.
 * 
 * @param str The input string.
 * @return The last character of the string.
 * 
 * @example
 * std::string text = "hello";
 * char lastChar = back(text);
 * // Result: 'o'
 */
char back(std::string str)
{
	return str[str.length() - 1];
}

/**
 * @brief Removes the last character from a string.
 * 
 * This function modifies the input string by removing its last character.
 * 
 * @param str The string to remove the last character from.
 * 
 * @example
 * std::string text = "hello";
 * pop_back(text);
 * // Result: text is now "hell"
 */
void pop_back(std::string& str)
{
	str.erase(str.length() - 1);
}

/**
 * @brief Retrieves the file extension from a filename.
 * 
 * This function returns the file extension from the given filename. If no extension is found or the filename ends with '.', 
 * an empty string is returned.
 * 
 * @param filename The filename from which to extract the extension.
 * @return The file extension or an empty string if not found.
 * 
 * @example
 * std::string file = "example.txt";
 * std::string extension = getExtension(file);
 * // Result: "txt"
 */
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

/**
 * @brief Converts a file extension to a corresponding MIME type.
 * 
 * This function takes a file extension as input and returns the corresponding MIME type.
 * If there is no extension or its not recognized, it defaults to "application/octet-stream".
 * 
 * @param filename The filename with the extension to convert.
 * @return The MIME type associated with the file extension.
 * 
 * @example
 * std::string file = "document.pdf";
 * std::string mimeType = extToMime(file);
 * // Result: "application/pdf"
 */
std::string extToMime(const std::string & filename)
{
	// Initialize the map if it's empty
	if (_ext_mime_map.empty())
		initext_mime_map();

	// Get the extension of the file
	std::string extension = getExtension(filename);

	// If the extension is not in the map, return "application/octet-stream"
	if (extension == "" || _ext_mime_map.count(extension) == 0)
		return "application/octet-stream";
	
	// Return the content type
	return _ext_mime_map[extension];
}

/**
 * @brief Converts a MIME type to the corresponding file extension.
 * 
 * This function takes a MIME type as input and returns the corresponding file extension.
 * If the MIME type is not recognized, it returns an empty string.
 * 
 * @param mime The MIME type to convert.
 * @return The file extension associated with the MIME type or an empty string if not found.
 * 
 * @example
 * std::string mimeType = "application/pdf";
 * std::string fileExtension = mimeToExt(mimeType);
 * // Result: "pdf"
 */
std::string mimeToExt(const std::string & mime)
{
	// Initialize the map if it's empty
	if (_ext_mime_map.empty())
		initext_mime_map();

	// Search the map for the mime type
	std::map<std::string,std::string>::iterator it;
	for (it = _ext_mime_map.begin(); it != _ext_mime_map.end(); ++it)
		if (it->second == mime)
			return it->first;

	// If the mime type is not in the map, return an empty string
	return "";
}

/**
 * @brief Checks if a file is binary based on its extension.
 * 
 * This function determines if a file is binary by examining the MIME associated with its extension.
 * If the MIME type starts with "text/", it is considered a non-binary file.
 * 
 * @param filename The name of the file to check.
 * @return True if the file is binary, false if it is a text file.
 * 
 * @example
 * std::string file = "image.png";
 * bool isBinary = isBinaryFile(file);
 * // Result: true
 */
bool isBinaryFile(const std::string & filename)
{
	std::string contentType = extToMime(filename);
	return (contentType.substr(0,5) != "text/");
}

/**
 * @brief Parses a string into a map of key-value pairs.
 * 
 * Each parameter is split by the specified separator, and each key-value pair is split by the equal sign.
 * 
 * @param inputString The input string to be parsed.
 * @param separator The separator used to split parameters.
 * @param eq The character used to split key-value pairs.
 * @return A map containing key-value pairs parsed from the input string.
 * 
 * @example
 * "key1=value1;key2=value2" -> {"key1": "value1", "key2": "value2"}
 */
std::map<std::string,std::string> get_params(const std::string & str, char separator, char eq)
{
	std::map<std::string,std::string> ret;

	size_t begin;
	size_t end = 0;
	do
	{
		// Update begin and end to the next param
		begin = end;
		end = str.find(separator, begin);

		// Get the param
		std::string param = str.substr(begin, end - begin);

		// If no eq, skip this param
		size_t eq_pos = param.find(eq);
		if (eq_pos != std::string::npos)
		{
			// Get the key and value, trim it and add it to the map
			std::string key = trim(param.substr(0, eq_pos));
			std::string value = trim(param.substr(eq_pos + 1));
			ret[key] = value;
		}

		// Add 1 to end to skip the separator
		if (end != std::string::npos)
			end++;
	} while (end != std::string::npos && end < str.length());

	return ret;
}

/**
 * @brief Removes surrounding quotes from a string.
 * 
 * This function checks if a string is enclosed in double quotes and removes them if present.
 * 
 * @param str The input string.
 * @return The string with surrounding quotes removed, or the original string if no quotes are found.
 * 
 * @example
 * std::string quotedStr = "\"example\"";
 * std::string unquotedStr = removeQuotes(quotedStr);
 * // Result: "example"
 */
std::string removeQuotes(const std::string& str)
{
	if (str.length() > 1 && str[0] == '"' && str[str.length() - 1] == '"')
		return str.substr(1, str.length() - 2);
	return str;
}

/**
 * @brief Decodes a URL-encoded string.
 *
 * Takes a URL-encoded string and decodes it, replacing percent-encoded
 * characters and converting '+' to space (' ').
 *
 * @param str The URL-encoded string to decode.
 * @return The decoded string.
 *
 * @note The function performs the following transformations:
 * - Replaces '+' with space (' ').
 * - Replaces '%xx' with the corresponding character, where 'xx' are hexadecimal digits.
 * - If 'xx' is not a valid hexadecimal representation, the '%' is left unchanged.
 *
 * @example
 * \code{.cpp}
 * std::string encoded = "Hello%20World%21";
 * std::string decoded = decodeURL(encoded);
 * // decoded now contains "Hello World!"
 * \endcode
 */
std::string decodeURL(std::string str)
{
	size_t strLen = str.length();
	std::string hex = "0123456789abcdef";

	// Replace '+' with ' '
	size_t pos = 0;
	while ((pos = str.find('+', pos)) != std::string::npos)
		str[pos] = ' ';

	// Replace '%xx' with the corresponding character
	pos = 0;
	while ((pos = str.find('%', pos)) != std::string::npos)
	{
		// If pos can't fit '%xx', break
		if (pos > strLen - 3)
			break;

		// If the next two characters are not hex digits, skip this '%'
		if (hex.find(tolower(str[pos + 1])) == std::string::npos
			|| hex.find(tolower(str[pos + 2])) == std::string::npos)
		{
			++pos;
			continue;
		}

		// Get the encoded character (first 4 bits are the first hex digit, last 4 bits are the second hex digit)
		char c = (hex.find(tolower(str[pos + 1])) << 4) | hex.find(tolower(str[pos + 2]));

		// Replace '%xx' with the decoded character
		str.replace(pos, 3, 1, c);
	}

	return str;
}
