#include "HttpRequest.hpp"
#include <sstream>
#include "utils.hpp"

/**
 * @brief Default Constructor for the HttpRequest class.
 * 
 * Initializes the request as not ready, the header as not ready, and no error.
 */
HttpRequest::HttpRequest()
{
	_requestReady = false;
	_headerReady = false;
	_error = false;
}

/**
 * @brief Constructor for the HttpRequest class.
 * 
 * Initializes the request with the input string. If the string contains a
 * complete request, the request is set as ready. Otherwise it gets added to
 * the buffer, waiting for more data.
 * 
 * If any error occurs, the error flag is set.
 * 
 * @param msg The string containing the http request or part of it.
 */
HttpRequest::HttpRequest(const std::string& request)
{
	_requestReady = false;
	_headerReady = false;
	_error = false;
	addData(request);
}

/**
 * @brief Copy Constructor for the HttpRequest class.
 * 
 * Initializes the request as a copy of another request.
 * 
 * @param other The HttpRequest to copy.
 */
HttpRequest::HttpRequest(const HttpRequest& other)
{
	*this = other;
}

/**
 * @brief Destructor for the HttpRequest class.
 * 
 * Does nothing.
 */
HttpRequest::~HttpRequest()
{}

/**
 * @brief Set the method of the request.
 * 
 * @param method The method to set.
 */
void HttpRequest::setMethod(const std::string& method)
{
	this->_method = method;
}

/**
 * @brief Get the method of the request.
 * 
 * @return The method of the request.
 */
const std::string & HttpRequest::getMethod() const
{
	return _method;
}

/**
 * @brief Set the path of the request.
 * 
 * If there is a query string, it is separated from the path and stored in
 * the _queryString attribute (use getQueryString() to get it).
 * 
 * The path and query string are decoded from URL formatting and the path is
 * cleaned from extra slashes or dots.
 * 
 * @param path The path to set.
 */
void HttpRequest::setPath(const std::string& path)
{
	size_t pos = path.find("?");
	if (pos != std::string::npos)
	{
		_queryString = path.substr(pos + 1);
		_path = path.substr(0, pos);
	}
	// Decode path and query string
	_path = decodeURL(_path);
	_queryString = decodeURL(_queryString);
	// Clean path
	_path = cleanPath(_path);

}

/**
 * @brief Get the path of the request.
 * 
 * If the path had a query string, it is not included in the path.
 * 
 * The returned path is decoded from URL formatting and cleaned from extra
 * slashes or dots.
 * 
 * @return The path of the request.
 */
const std::string & HttpRequest::getPath() const
{
	return _path;
}

/**
 * @brief Get the query string of the request.
 * 
 * The query string is decoded from URL formatting.
 * 
 * Does not include the ? character.
 * 
 * @return The query string of the request.
 */
const std::string & HttpRequest::getQueryString() const
{
	return _queryString;
}

/**
 * @brief Set a header in the request.
 * 
 * Multiple headers with the same key are allowed.
 * 
 * If a header with the same key already exists, it is not replaced.
 * 
 * @param key The key of the header.
 * @param value The value of the header.
 * 
 * @example setHeader("Content-Type", "text/html");
 */
void HttpRequest::setHeader(const std::string& key, const std::string& value)
{
	// Multiple headers with the same key are allowed
	_headers.push_back(std::make_pair(key, value));
}

/**
 * @brief Unset a header name from the request.
 * 
 * Removes all headers with the given key.
 * 
 * @param key The key of the header to remove.
 */
void HttpRequest::unsetHeader(const std::string& key)
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
std::vector<std::string> HttpRequest::getHeader(const std::string& key) const
{
	std::vector<std::string> output;

	size_t header_size = _headers.size();
	for (size_t i = 0; i < header_size; i++)
		if (_headers[i].first == key)
			output.push_back(_headers[i].second);
	return output;
}

/**
 * @brief Get all headers of the request.
 * 
 * Maybe used to iterate over all headers from outside the class.
 * 
 * @return A reference to the headers of the request.
 */
const std::vector<std::pair<std::string, std::string> > & HttpRequest::getHeaders() const
{
	return _headers;
}

/**
 * @brief Set the body of the request.
 * 
 * The body is set as is, no decoding or cleaning is done.
 * 
 * The headers Content-Length and Transfer-Encoding are updated accordingly.
 * 
 * @param body The body to set.
 * @param chunked (default = false) If true, the body is sent as chunked.
 */
void HttpRequest::setBody(const std::string& body, bool chunked)
{
	this->_body = body;
	unsetHeader("Transfer-Encoding");
	unsetHeader("Content-Length");
	if (chunked)
		setHeader("Transfer-Encoding", "chunked");
	else
		setHeader("Content-Length", intToString(body.size()));
}

/**
 * @brief Set the request as ready or not.
 * 
 * Used when manually setting the request. If the request is set as ready,
 * it is considered complete and can be sent.
 * 
 * Don't use this function unless you know what you are doing!
 * @param ready The value to set.
 */
void HttpRequest::requestReady(bool ready)
{
	_requestReady = ready;
}

/**
 * @brief Check if the request is ready.
 * 
 * @return True if the request is ready, false otherwise.
 */
bool HttpRequest::requestReady() const
{
	return _requestReady;
}

/**
 * @brief Get the body of the request.
 * 
 * @return The body of the request.
 */
const std::string & HttpRequest::getBody() const
{
	return _body;
}

/**
 * @brief Extract a chunk from the buffer and add it to the body.
 * 
 * If the buffer is not long enough to read a chunk, return 0.
 * 
 * If there is any error reading the chunk, return -1.
 * 
 * If the chunk is read successfully, return the size of the chunk read.
 * 
 *
 * @param buf The buffer to read from
 * @param body The body to add the chunk to
 * @return The size of the chunk read
 */
static long long int getChunk(std::string & buf, std::string & body)
{
	//get the length of the chunk
	size_t chunkSize;
	size_t pos = buf.find("\r\n");
	// if the buff is not long enough return 0, else if it is more than 8HEX + \r return -1
	if (pos == std::string::npos)
		return (buf.size() >= 10 ? -1 : 0);
	std::istringstream iss(buf.substr(0, pos));
	iss >> std::hex >> chunkSize;
	if (iss.fail() or not iss.eof())
		return -1;

	// if the buffer is not long enough, skip
	// hex size + \r\n + chunkSize + \r\n
	if (buf.size() < pos + chunkSize + 4)
		return 0;
	
	// add the chunk to the body and remove it from the buffer
	body += buf.substr(pos + 2, chunkSize);
	buf = buf.substr(pos + chunkSize + 4);

	// return the read size
	return pos + chunkSize + 4;
}

/**
 * @brief Add a piece of data to the request.
 * 
 * Main function used to parse the request. If the data is long enough to
 * contain a complete request, only one call is needed. Otherwise, multiple
 * calls are needed.
 * 
 * Can handle chunked encoding.
 * 
 * Keeps parsing data from the input until the request is ready.
 * 
 * Returns the number of bytes read from the input each time it is called.
 * 
 * If the request is ready, it returns 0 because it doesn't read anything.
 * 
 * If an error occurs, it returns -1.
 * 
 * @param data The data to add to the request.
 * @return The number of bytes read.
 */
long long int HttpRequest::addData(const std::string & data)
{
	if (_requestReady)
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
				_requestReady = true;
		}
		//is chunked
		else
		{
			long long int chunkSize;
			do
			{
				chunkSize=getChunk(_inBuff, _body);
				// 0\r\n\r\n
				if (chunkSize == 5)
				{
					_requestReady = true;
					unsetHeader("Transfer-Encoding");
					unsetHeader("Content-Length");
					setHeader("Content-Length", intToString(_body.size()));
				}
				else if (chunkSize == -1)
					return (this->clear(), _error=true, -1);
			} while (chunkSize > 5);
		}
	}

	long long int ret = data.size() - _inBuff.size();
	if (_requestReady)
		_inBuff.clear();
	return ret; 
}

/**
 * @brief Convert the request to a string.
 * 
 * Used to create a string from the request to send it.
 * If the request is not ready, an empty string is returned.
 * 
 * @return The request as a string.
 */
std::string HttpRequest::to_string() const
{
	if (!_requestReady)
		return "";

	std::string ret;
	if (_queryString.empty())
		ret = _method + " " + _path + " HTTP/1.1\r\n";
	else
		ret = _method + " " + _path + "?" + _queryString + "HTTP/1.1\r\n";

	for (size_t i = 0; i < _headers.size(); ++i)
		ret += _headers[i].first + ": " + _headers[i].second + "\r\n";
	ret += "\r\n" + _body;

	return ret;
}

/**
 * @brief Clear all fields of the request.
 * 
 * Used to reset the request to its initial state.
 */
void HttpRequest::clear()
{
	_method.clear();
	_path.clear();
	_queryString.clear();
	_headers.clear();
	_body.clear();
	_requestReady = false;
	_headerReady = false;
	_error = false;
	_inBuff.clear();
}

/**
 * @brief Check if the request is in an error state.
 * 
 * @return True if the request is in an error state, false otherwise.
 */
bool HttpRequest::error() const
{
	return _error;
}

/**
 * @brief Copy the request.
 * 
 * @param rhs The request to copy.
 * @return A reference to the copied request.
 */
HttpRequest & HttpRequest::operator=(HttpRequest const & rhs)
{
	if (this != &rhs)
	{
		this->_method = rhs._method;
		this->_path = rhs._path;
		this->_queryString = rhs._queryString;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
		this->_requestReady = rhs._requestReady;
		this->_headerReady = rhs._headerReady;
		this->_error = rhs._error;
		this->_inBuff = rhs._inBuff;
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
int HttpRequest::parseHeader(const std::string& header)
{
	_headers.clear();

	std::istringstream iss(header);

	// Get first line
	std::string version;
	std::string line;
	std::getline(iss, line);
	std::istringstream iss_line(trim(line));
	iss_line >> _method >> _path >> version;
	// Check if line is valid
	if (iss_line.fail() || !iss_line.eof())
		return 1;
	// Use setPath to parse the path and query string
	setPath(_path);


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
 * @brief Class printf!
 * 
 * Used to print the request to the output stream.
 * 
 * If the request is ready, it prints the request as a string.
 * Otherwise, it prints the attributes of the request.
 * 
 * @param o The output stream to print to.
 * @param rhs The request to print.
 * @return The output stream.
 */
std::ostream & operator<<(std::ostream & o, HttpRequest const & rhs)
{
	if (rhs._requestReady)
	{
		o << rhs.to_string();
		return o;
	}
	o << "_method: " << rhs._method << std::endl;
	o << "_path: " << rhs._path << std::endl;
	o << "_queryString: " << rhs._queryString << std::endl;
	o << "_headers: " << rhs._headers.size() << std::endl;
	for (size_t i = 0; i < rhs._headers.size(); i++)
		o << "\t" << rhs._headers[i].first << ": " << rhs._headers[i].second << std::endl;
	o << "_body: " << rhs._body << std::endl;
	o << "_inBuff: " << rhs._inBuff << std::endl;
	o << "_requestReady: " << rhs._requestReady << std::endl;
	o << "_headerReady: " << rhs._headerReady << std::endl;
	o << "_error: " << rhs._error << std::endl;
	return o;
}