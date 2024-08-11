#include "HttpResponse.hpp"
#include <sstream>
#include "utils.hpp"
#include <fstream>

/**
 * @brief Default constructor for HttpResponse objects.
 * 
 * Initializes the response as not ready, the header as not ready, and no error.
 */
HttpResponse::HttpResponse()
{
	_responseReady = false;
	_error = false;
}

/**
 * @brief Constructor for the HttpResponse class.
 * 
 * Initializes the response with the input string. If the string contains a
 * complete response, the response is set as ready. Otherwise it gets added to
 * the buffer, waiting for more data.
 * 
 * If any error occurs, the error flag is set.
 * 
 * @param msg The string containing the http request or part of it.
 */
HttpResponse::HttpResponse(const std::string & response)
{
	_responseReady = false;
	_error = false;
	addData(response);
}

/**
 * @brief Copy constructor for HttpResponse objects.
 * 
 * Initializes the response as a copy of another response.
 * 
 * @param other The HttpResponse object to be copied.
 */
HttpResponse::HttpResponse(const HttpResponse& other)
{
	*this = other;
}

/**
 * @brief Destructor for HttpResponse objects.
 * 
 * Does nothing.
 */
HttpResponse::~HttpResponse()
{}

/**
 * @brief Set the status code and phrase of the response.
 * 
 * Sets the status code and phrase of the response.
 * 
 * @param code The status code.
 * @param phrase (optional) The status phrase.
 */
void HttpResponse::setStatus(int code, const std::string& phrase)
{
	_statusCode = intToString(code);
	_statusPhrase = phrase;
}

/**
 * @brief Get the status code of the response.
 * 
 * Returns the status code of the response.
 * 
 * @return The status code.
 */
const std::string & HttpResponse::getStatusCode() const
{
	return _statusCode;
}

/**
 * @brief Get the status phrase of the response.
 * 
 * Returns the status phrase of the response.
 * 
 * @return The status phrase.
 */
const std::string & HttpResponse::getStatusPhrase() const
{
	return _statusPhrase;
}

/**
 * @brief Set a header in the response.
 * 
 * Multiple headers with the same key are allowed.
 * 
 * If a header with the same key already exists, it is not replaced.
 * 
 * @param key The key of the header.
 * @param value The value of the header.
 */
void HttpResponse::setHeader(const std::string& key, const std::string& value)
{
	_headers.push_back(std::make_pair(key, value));
}

/**
 * @brief Unset a header name from the response.
 * 
 * Removes all headers with the given key.
 * 
 * @param key The key of the header to remove.
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
 * @brief Get all headers with a given key.
 * 
 * @param key The key of the headers to get.
 * 
 * @return A vector with the contents of every header with the key.
 */
std::vector<std::string> HttpResponse::getHeader(const std::string& key) const
{
	std::vector<std::string> ret;
	std::vector<std::pair<std::string, std::string> >::const_iterator it;
	for (it = _headers.begin(); it != _headers.end(); ++it)
		if (it->first == key)
			ret.push_back(it->second);
	return ret;
}

/**
 * @brief Get all headers of the response.
 * 
 * Maybe used to iterate over all headers from outside the class.
 * 
 * @return A reference to the headers of the response.
 */
const std::vector<std::pair<std::string, std::string> > & HttpResponse::getHeaders() const
{
	return _headers;
}

/**
 * @brief Set the body of the response.
 * 
 * Sets the body of the response with the given content type and body.
 * 
 * The headers Content-Type, Content-Length and Transfer-Encoding are set
 * accordingly.
 * 
 * If the body is empty, the Content-Type header is not set. So to set an empty
 * body, use an empty content_type and body as inputs.
 * 
 * @param content_type The content type of the body.
 * @param body The body of the response.
 * @param chunked (default=false) True if the body is in chunked format.
 */
void HttpResponse::setBody(const std::string& body, bool chunked)
{
	_body = body;
	unsetHeader("Content-Length");
	unsetHeader("Transfer-Encoding");
	if (chunked)
		setHeader("Transfer-Encoding", "chunked");
	else
		setHeader("Content-Length", intToString(_body.size()));
}

/**
 * @brief Get the body of the response.
 * 
 * @return The body of the response.
 */
const std::string & HttpResponse::getBody() const
{
	return (_body);
}

/**
 * @brief Set the response as ready.
 * 
 * @param ready True if the response is ready, false otherwise.
 */
void HttpResponse::responseReady(bool ready)
{
	_responseReady = ready;
}

/**
 * @brief Check if the response is ready.
 * 
 * @return True if the response is ready, false otherwise.
 */
bool HttpResponse::responseReady() const
{
	return _responseReady;
}

/**
 * @brief Add a piece of data to the response.
 * 
 * Main function used to parse the response. If the data is long enough to
 * contain a complete response, only one call is needed. Otherwise, multiple
 * calls are needed.
 * 
 * Keeps parsing data from the input until the resopnse is ready.
 * 
 * Returns the number of bytes read from the input each time it is called.
 * 
 * If the response is ready, it returns 0 because it doesn't read anything.
 * 
 * If an error occurs, it returns -1.
 * 
 * @param data The data to add to the response.
 * @return The number of bytes read.
 */
long long int HttpResponse::addData(const std::string & data)
{
	if (_responseReady)
		return 0;

	_inBuff = _inBuff + data;
	// If the header has not been found yet, search for it
	if (not _headerReady)
	{
		// Find the end of the header
		size_t pos = _inBuff.find("\r\n\r\n");
		// If the end of the header is not found, return the number of bytes read
		if (pos == std::string::npos)
			return data.size();
		// If the end of the header is found, parse it
		if (parseHeader(_inBuff.substr(0, pos + 4)) == 1)
			return (this->clear(), _error=true, -1);
		_inBuff = _inBuff.substr(pos + 4);
		_headerReady = true;
	}
	// Parse the body if the header has been found already
	// (Use if instead of else bc body can be parsed after parsing header)
	if (_headerReady)
	{
		std::vector<std::string> chunkedHeader = getHeader("Transfer-Encoding");
		// not chunked
		if (chunkedHeader.size() == 0 or chunkedHeader[0] != "chunked")
		{
			// Get the content length
			std::vector<std::string> contentLengthHeader = getHeader("Content-Length");
			size_t contentLength = 0;
			if (contentLengthHeader.size() > 0)
			{
				std::istringstream iss(contentLengthHeader[0]);
				iss >> contentLength;
				if (iss.fail() or not iss.eof())
					return (this->clear(), _error=true, -1);
			}
			// If no content length is found, or some weird error occurs, return
			if (contentLength < _body.size())
				return (this->clear(), _error=true, -1);
			// Add the data to the body
			if (contentLength != 0)
			{
				size_t len = (contentLength - _body.size() > _inBuff.size()) ?
					_inBuff.size() : contentLength - _body.size();
				_body += _inBuff.substr(0, len);
				_inBuff = _inBuff.substr(len);
			}
			// If the body is complete, set the request as ready
			if (_body.size() == contentLength)
				_responseReady = true;
		}
		//is chunked
		else
		{
			size_t pos = _inBuff.find("0\r\n\r\n");
			if (pos != std::string::npos)
			{
				_responseReady = true;
				_body += _inBuff.substr(0, pos + 5);
				_inBuff = _inBuff.substr(pos + 5);
			}
		}
	}

	long long int ret = data.size() - _inBuff.size();
	if (_responseReady)
		_inBuff.clear();
	return ret; 
}

/**
 * @brief Get the string representation of the response.
 * 
 * @return The string representation of the response.
 */
std::string HttpResponse::to_string() const
{
	if (_statusCode == "")
		return ("");

	std::string output;
	output += "HTTP/1.1 " + _statusCode + " " + _statusPhrase + "\r\n";
	size_t header_size = _headers.size();
	for (size_t i = 0; i < header_size; i++)
		output += _headers[i].first + ": " + _headers[i].second + "\r\n";
	output += "\r\n" + _body;
	return (output);
}

/**
 * @brief Clear the response.
 * 
 * Clears the response, setting it as not ready, the header as not ready, and no error.
 */
void HttpResponse::clear()
{
	_statusCode.clear();
	_statusPhrase.clear();
	_headers.clear();
	_body.clear();
	_responseReady = false;
	_headerReady = false;
	_error = false;
	_inBuff.clear();
}

/**
 * @brief Check if the response has errors.
 * 
 * @return True if the response has errors, false otherwise.
 */
bool HttpResponse::error() const
{
	return _error;
}

/**
 * @brief Generate a default response.
 * 
 * Generates an error response with the given status code and phrase.
 * 
 * If no message is given, a default message is used.
 * 
 * @param code The status code of the error.
 * @param phrase The status phrase of the error.
 * @param msg (optional) The message of the error.
 * @return The error response.
 */
HttpResponse HttpResponse::errorResponse(int code, const std::string & phrase, std::string msg)
{
	HttpResponse ret;

	// default error pages
	if (msg.empty())
	{
		switch (code)
		{
			case 400:
				msg = "Bad request.";
				break;
			case 401:
				msg = "Unauthorized.";
				break;
			case 404:
				msg = "The requested resource was not found.";
				break;
			case 403:
				msg = "Access denied.";
				break;
			case 405:
				msg = "Method not allowed.";
				break;
			case 500:
				msg = "Internal server error.";
				break;
			case 501:
				msg = "Not implemented.";
				break;
		}
	}

	// Set the status code and phrase
	ret.setStatus(code, phrase);
	ret.setHeader("Content-Type", "text/html");
	if (msg.length() == 0)
		ret.setBody("<html><body><h1>" + ret.getStatusCode() + " "
			+ ret.getStatusPhrase() + "</h1></body></html>");
	else
		ret.setBody("<html><body><h1>" + ret.getStatusCode() + " "
			+ ret.getStatusPhrase() + "</h1><p>" + msg + "</p></body></html>");
	ret._responseReady = true;
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
		_statusCode = rhs._statusCode;
		_statusPhrase = rhs._statusPhrase;
		_headers = rhs._headers;
		_body = rhs._body;
		_responseReady = rhs._responseReady;
		_error = rhs._error;
	}
	return *this;
}

/**
 * @brief Parse the header of the request.
 * 
 * Auxiliary function used to parse the header of the request.
 * Used only by addData.
 * 
 * @param header The header to parse.
 * @return 0 if the header is parsed successfully, 1 otherwise.
 */
int HttpResponse::parseHeader(const std::string& header)
{
	_headers.clear();

	std::istringstream iss(header);

	// Get first line
	std::string version;
	std::string line;
	std::getline(iss, line);
	std::istringstream iss_line(trim(line));
	iss_line >> version >> _statusCode;
	// Get the rest of the line to _statusPhrase
	std::getline(iss_line, _statusPhrase);
	_statusPhrase = trim(_statusPhrase);
	if (iss_line.fail())
		return 1;
	// Check if the _statusCode is ok
	for (size_t i = 0; i < _statusCode.size(); ++i)
		if (_statusCode[i] < '0' or _statusCode[i] > '9')
			return 1;
	if (_statusCode.size() > 3 or _statusCode[0] > '5')
		return 1;

	// Get headers
	while (std::getline(iss, line) && line != "\r" && line != "")
	{
		// Check if line is valid
		size_t index = line.find(':');
		if (index == std::string::npos)
			return 1;
		// Parse line
		std::string key = trim(line.substr(0, index));
		std::string value = trim(line.substr(index + 1));
		_headers.push_back(std::make_pair(key, value));
	}

	return 0;
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
	if (rhs._responseReady)
	{
		o << rhs.to_string();
		return o;
	}
	o << "_statusCode: " << rhs._statusCode << std::endl;
	o << "_statusPhrase: " << rhs._statusPhrase << std::endl;
	o << "_headers: " << rhs._headers.size() << std::endl;
	for (size_t i = 0; i < rhs._headers.size(); i++)
		o << "\t" << rhs._headers[i].first << ": " << rhs._headers[i].second << std::endl;
	o << "_body (" << rhs._body.size() << "): " << rhs._body << std::endl;
	o << "_inBuff: " << rhs._inBuff << std::endl;
	o << "_responseReady: " << rhs._responseReady << std::endl;
	o << "_headerReady: " << rhs._headerReady << std::endl;
	o << "_error: " << rhs._error << std::endl;
	return o;
}