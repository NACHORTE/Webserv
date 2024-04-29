#include "HttpResponse.hpp"
#include <sstream>
#include "utils.hpp"
#include <map>
#include <fstream>
#include "colors.h"

std::map<int, std::string> _errorPages;
extern int g_count;
/**
 * @brief Initializes the static error pages map in the HttpResponse class.
 * 
 * Checks if the error pages map is already initialized. If not, populates it with
 * default error pages for HTTP status codes 404 and 500.
 */
static void initErrorPages()
{
	if (_errorPages.size() > 0)
		return;
	_errorPages[404] = "./html/404.html";
	_errorPages[500] = "./html/500.html";
}

HttpResponse::HttpResponse()
{
	_responseReady = false;
}

HttpResponse::HttpResponse(const HttpResponse& other)
{
	*this = other;
}

/**
 * @brief
 * Initializes an HttpResponse object by generating a response using the provided HttpRequest,
 * valid paths, and valid methods.
 * 
 * @param req The HttpRequest object representing the incoming request.
 * @param valid_paths The Locations object containing valid paths and their associated methods.
 * @param valid_methods A map of valid methods along with their corresponding functions.
 * 
 * @see generate
 */
HttpResponse::HttpResponse(
	const HttpRequest& req,
	const Locations & valid_paths,
	const std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)> & valid_methods)
{
	generate(req, valid_paths, valid_methods);
}

HttpResponse::~HttpResponse()
{}

/**
 * @brief
 * Sets the status code and phrase of the HttpResponse based on the provided values.
 * If the provided phrase is empty, it determines a default phrase based on the status code range.
 * 
 * @param code The HTTP status code to set.
 * @param phrase The HTTP status phrase to set.
 */
void HttpResponse::setStatus(int code, const std::string& phrase)
{
	this->_status_code = int_to_string(code);
	if (phrase.length() > 0)
		this->_status_phrase = phrase;
	else
	{
		if (code >= 200 && code < 300)
			this->_status_phrase = "OK";
		else if (code >=400 && code < 500)
			this->_status_phrase = "Not Found";
		else if (code >= 500)
			this->_status_phrase = "Internal Server Error";
		else
			this->_status_phrase = "";
	}
}

/**
 * @brief Sets a header with the specified key and value in the HttpResponse.
 * Multiple headers with the same key are allowed.
 * 
 * @param key The header key to be set.
 * @param value The value to associate with the header key.
 */
void HttpResponse::setHeader(const std::string& key, const std::string& value)
{
	// Multiple headers with the same key are allowed
	_headers.push_back(std::make_pair(key, value));
}

/**
 * @brief
 * Removes all occurrences of a header with the specified key from the HttpResponse.
 * 
 * @param key The header key to be unset.
 * 
 */
void HttpResponse::unsetHeader(const std::string& key)
{
	std::vector<std::pair<std::string, std::string> >::iterator it;
	for (it = _headers.begin(); it != _headers.end();)
	{
		if (it->first == key)
			it = _headers.erase(it);
		else
			++it;
	}
}

/**
 * @brief Sets the body of the HttpResponse with the specified content type and body content.
 * 
 * Sets the body of the HttpResponse using the provided content type and body content.
 * Updates the "Content-Type" and "Content-Length" headers accordingly. If the body
 * is non-empty, it sets the headers; otherwise, it unsets them.
 * 
 * @param content_type The Content-Type to set for the response.
 * @param body The body content to set.
 * 
 * @see setHeader
 * @see unsetHeader
 * @see int_to_string
 */
void HttpResponse::setBody(const std::string& content_type, const std::string& body)
{
	this->_body = body;
	unsetHeader("Content-Type");
	unsetHeader("Content-Length");
	if (_body.length() > 0)
	{
		setHeader("Content-Type", content_type);
		setHeader("Content-Length", int_to_string(_body.length()));
	}
}

/**
 * @brief Sets the body of the HttpResponse with the content of a file.
 * 
 * This function sets the body of the HttpResponse using the content of a file specified
 * by the provided filename. It also sets the Content-Type header based on the file extension
 * and calculates the Content-Length.
 * 
 * @param filename The name of the file whose content will be set as the body.
 * 
 * @throws std::runtime_error if the file cannot be opened.
 * 
 * @see extToMime
 * @see readFile
 * @see isBinaryFile
 */
void HttpResponse::setBody(const std::string& filename)
{
	setBody(extToMime(filename), readFile(filename, isBinaryFile(filename)));
}

/**
 * @brief Gets the status code of the HttpResponse.
 * 
 * Returns a constant reference to the status code of the HttpResponse.
 * 
 * @return A constant reference to the status code.
 */
const std::string & HttpResponse::getStatusCode() const
{
	return _status_code;
}

/**
 * @brief Gets the status phrase of the HttpResponse.
 * 
 * Returns a constant reference to the status phrase of the HttpResponse.
 * 
 * @return A constant reference to the status phrase.
 */
const std::string & HttpResponse::getStatusPhrase() const
{
	return _status_phrase;
}

/**
 * @brief Retrieves all values for a specified header key in the HttpResponse.
 * 
 * Returns a vector containing all values associated with the specified header key
 * in the HttpResponse object.
 * 
 * @param key The header key for which values are to be retrieved.
 * @return A vector of strings containing all values associated with the specified header key.
 */
std::vector<std::string> HttpResponse::getHeader(const std::string& key) const
{
	std::vector<std::string> ret;
	std::vector<std::pair<std::string, std::string> >::const_iterator it;
	for (it = _headers.begin(); it != _headers.end(); ++it)
	{
		if (it->first == key)
			ret.push_back(it->second);
	}
	return ret;
}

/**
 * @brief Gets the body of the HttpResponse.
 * 
 * Returns a constant reference to the body content of the HttpResponse.
 * 
 * @return A constant reference to the body content.
 */
const std::string & HttpResponse::getBody() const
{
	return (_body);
}

/**
 * @brief Clears the HttpResponse object.
 * 
 * This function clears the HttpResponse object by setting its status code, status phrase,
 * headers, and body to empty strings.
 */
void HttpResponse::clear()
{
	_status_code.clear();
	_status_phrase.clear();
	_headers.clear();
	_body.clear();
}

/**
 * @brief Checks if the HttpResponse object is empty.
 * 
 * Determines whether the HttpResponse object is empty by checking if
 * its status code, status phrase, headers, and body are all empty.
 * 
 * @return true if the HttpResponse is empty, false otherwise.
 */
bool HttpResponse::empty() const
{
	return (_status_code.empty()
		&& _status_phrase.empty()
		&& _headers.empty()
		&& _body.empty());
}

/**
 * Generates and returns a string representation of the HttpResponse object
 * in the format of an HTTP response. It includes the status line, headers, and body.
 * 
 * @return A string representation of the HttpResponse.
 * 
 * @note If the status code is empty, an empty string is returned.
 */
std::string HttpResponse::to_string() const
{
	if (_status_code == "")
		return ("");

	std::string output;
	output += "HTTP/1.1 " + _status_code + " " + _status_phrase + "\r\n";
	size_t header_size = _headers.size();
	for (size_t i = 0; i < header_size; i++)
		output += _headers[i].first + ": " + _headers[i].second + "\r\n";
	output += "\r\n" + _body;
	return (output);
}

/**
 * Generates an HttpResponse object by processing the given HttpRequest, validating
 * the request method and path against provided constraints. It utilizes a map of valid methods to
 * determine the appropriate action. If the request method or path is not valid, it returns an
 * HTTP response with the corresponding error status code.
 * 
 * @param req The HttpRequest object representing the incoming request.
 * @param valid_paths The Locations object containing valid paths and their associated methods.
 * @param valid_methods A map of valid methods along with their corresponding functions.
 * 
 * @note The function relies on the error() function to generate error responses when necessary.
 * 
 * @see error
 */
void HttpResponse::generate(
	const HttpRequest & req,
	const Locations & valid_paths,
	const std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)> & valid_methods)
{
	// Empty everything
	clear();

	// If method is not valid, return 405 Method Not Allowed
	if (valid_methods.count(req.get_method()) == 0)
		return (void)(*this = error(405, "Method Not Allowed"), _responseReady = true);

	// If path does not exist, return 404 Not Found
	if (valid_paths.pathExists(req.get_path()) == false)
		return (void)(*this = error(404, "Not Found"), _responseReady = true);

	// If request is not valid, return 403 Forbidden
	if (valid_paths.isPathAllowed(req.get_method(), req.get_path()) == false)
		return (void)(*this = error(403, "Forbidden"), _responseReady = true);

	// Generate response with the appropriate method
	try
	{
		*this = valid_methods.at(req.get_method())(req, valid_paths);
	}
	catch (std::exception & e)
	{
		*this = error(500, "Internal Server Error", e.what());
	}

	_responseReady = true;
}

/**
 * @brief Generates an HTTP response for a specified error code.
 * 
 * This function creates an HttpResponse object for a given error code. It attempts to retrieve
 * an associated error page from a predefined map based on the error code. If a corresponding
 * error page is found, it sets the HTTP response body using the content of that file. If no
 * associated error page is found, it generates a generic error page with the provided message.
 * 
 * @param code The HTTP error code.
 * @param phrase The HTTP status phrase.
 * @param msg The additional message to include in the error page.
 * @return An HttpResponse object representing the error response.
 * 
 * @note The function relies on the extToMime() function to determine the MIME type
 *       of the error page content.
 * 
 * @see extToMime
 * 
 * @example
 * HttpResponse response = HttpResponse::error(404, "Not Found", "The requested resource was not found.");
 * // Result: An HttpResponse object with appropriate status, error page, and message.
 */
HttpResponse HttpResponse::error(	// generate a default error page
			int code,
			const std::string & phrase,
			const std::string & msg)
{
	HttpResponse ret;

	// Initialize the error pages map
	initErrorPages();

	// Set the status code and phrase
	ret.setStatus(code, phrase);
	
	// try to get the error page first from the error pages map
	try
	{
		std::string filename = _errorPages.at(code);
		ret.setBody(extToMime(filename),readFile(filename, isBinaryFile(filename)));
	}
	// if it fails, return a generic error page
	catch(const std::exception& e)
	{
		if (msg.length() == 0)
			ret.setBody("text/html", "<html><body><h1>" + ret.getStatusCode() + " "
				+ ret.getStatusPhrase() + "</h1></body></html>");
		else
			ret.setBody("text/html", "<html><body><h1>" + ret.getStatusCode() + " "
				+ ret.getStatusPhrase() + "</h1><p>" + msg + "</p></body></html>");
	}

	return ret;	
}

/**
 * @brief Checks if the HttpResponse object has a response ready.
 * 
 * Determines whether the HttpResponse object has a response ready by checking the _responseReady flag.
 * 
 * @return true if the response is ready, false otherwise.
 */
bool HttpResponse::responseReady() const
{
	return _responseReady;
}

/**
 * @brief Retrieves a portion of the response buffer.
 * 
 * This function retrieves a portion of the response buffer with a specified length.
 * If the length is -1, it returns the entire response buffer.
 * 
 * @param length The length of the response buffer to retrieve.
 * @return A string containing the specified portion of the response buffer.
 */
std::string HttpResponse::popResponse(size_t length)
{
	std::string ret;
	ret = _responseBuffer.substr(0, length);
	if ( length >= _responseBuffer.length() )
		_responseBuffer.clear();
	else
		_responseBuffer = _responseBuffer.substr(length);
	return ret;
}

/**
 * @brief Overloads the assignment operator for HttpResponse objects.
 * 
 * Overloads the assignment operator to copy the content of another HttpResponse object.
 * 
 * @param rhs The HttpResponse object to be copied.
 * @return A reference to the modified HttpResponse object.
 */
HttpResponse & HttpResponse::operator=(const HttpResponse& rhs)
{
	if (this != &rhs)
	{
		this->_status_code = rhs._status_code;
		this->_status_phrase = rhs._status_phrase;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
		this->_responseBuffer = rhs._responseBuffer;
		this->_responseReady = rhs._responseReady;
	}
	return *this;
}

/**
 * @brief Overloads the () operator to get the string representation of the HttpResponse.
 * 
 * Overloads the () operator to provide a convenient way to obtain the string representation
 * of the HttpResponse using the to_string() function.
 * 
 * @return The string representation of the HttpResponse.
 * 
 * @see to_string
 */
std::string HttpResponse::operator()() const
{
	return to_string();
}

/**
 * @brief Overloads the << operator for streaming an HttpResponse to an ostream.
 * 
 * Overloads the << operator to allow streaming the string representation of an
 * HttpResponse to an ostream.
 * 
 * @param o The ostream to which the HttpResponse will be streamed.
 * @param rhs The HttpResponse object to be streamed.
 * @return A reference to the ostream.
 * 
 * @see to_string
 */
std::ostream & operator<<(std::ostream & o, const HttpResponse& rhs)
{
	o << rhs.to_string();
	return o;
}