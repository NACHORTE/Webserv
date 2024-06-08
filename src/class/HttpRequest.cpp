#include "HttpRequest.hpp"
#include <sstream>
#include "utils.hpp"
#include "colors.h"

HttpRequest::HttpRequest()
{
	_requestReady = false;
	_headerReady = false;
	_error = false;
}

HttpRequest::HttpRequest(const std::string& msg)
{
	_requestReady = false;
	_headerReady = false;
	_error = false;
	parse(msg);
}

HttpRequest::HttpRequest(const HttpRequest& other)
{
	*this = other;
}

HttpRequest::~HttpRequest()
{}

void HttpRequest::set_method(const std::string& method)
{
	this->_method = method;
}

void HttpRequest::set_path(const std::string& path)
{
	this->_path = path;
}

void HttpRequest::set_version(const std::string& version)
{
	this->_version = version;
}

void HttpRequest::setHeader(const std::string& key, const std::string& value)
{
	// Multiple headers with the same key are allowed
	_headers.push_back(std::make_pair(key, value));
}

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

void HttpRequest::setBody(const std::string& body)
{
	this->_body = body;
}

size_t HttpRequest::addData(const std::string & data)
{
	// If the request is ready, return 0 bytes read
	if (_requestReady)
		return 0;
	// Create auxiliar variable with the buffer and data
	std::string msg = _buffer + data;
	// Variable to store the number of bytes read from data
	size_t readBytes = 0;
	// Save old length of buffer and empty it
	size_t oldBuffLen = _buffer.size();
	_buffer.clear();
	// If the header has not been found yet, search for it
	if (!_headerReady)
	{
		// Find the end of the header
		size_t pos = msg.find("\r\n\r\n");
		// If end of header has not been found, save the buffer and return
		if (pos == std::string::npos)
		{
			_buffer = msg;
			return data.size();
		}
		// If the end of the header is found, parse it
		if (parseHeader(msg.substr(0, pos + 4)) == 1)
			return (_error = true, 0);
		msg = msg.substr(pos + 4);
		readBytes = pos + 4 - oldBuffLen;
		_headerReady = true;
	}
	// Parse the body if the header has been found already
	// (Use if instead of else bc body can be parsed after parsing header)
	if (_headerReady)
	{
		// Check if the message is chunked
		std::vector<std::string> chunkedHeader = getHeader("Transfer-Encoding");
		if (chunkedHeader.size() == 0 || chunkedHeader[0] != "chunked")
		{
			// Get the content length
			std::vector<std::string> contentLengthHeader = getHeader("Content-Length");
			size_t contentLength = 0;
			if (contentLengthHeader.size() > 0)
			{
				std::istringstream iss(contentLengthHeader[0]);
				iss >> contentLength;
				if (iss.fail() || !iss.eof())
					return (_error = true, 0);
			}

			// Add the data to the body
			size_t oldBodySize = _body.size();
			_body += msg.substr(0, contentLength - oldBodySize);
			readBytes += (contentLength - oldBodySize > msg.size()) ? msg.size() : contentLength - oldBodySize;
			// If the body is complete, set the request as ready
			if (_body.size() == contentLength)
			{
				_requestReady = true;
				return readBytes;
			}
		}
		else
		{
			//is chunked
		}
	}

	return readBytes; 
}

const std::string & HttpRequest::getMethod() const
{
	return _method;
}

const std::string & HttpRequest::getPath() const
{
	return _path;
}

const std::string & HttpRequest::getVersion() const
{
	return _version;
}

std::vector<std::string> HttpRequest::getHeader(const std::string& key) const
{
	std::vector<std::string> output;

	size_t header_size = _headers.size();
	for (size_t i = 0; i < header_size; i++)
		if (_headers[i].first == key)
			output.push_back(_headers[i].second);
	return output;
}

std::vector<std::pair<std::string, std::string> > HttpRequest::getHeaders() const
{
	return _headers;
}

const std::string & HttpRequest::getBody() const
{
	return _body;
}

void HttpRequest::parse(const std::string& msg)
{
	// Clear current request
	this->clear();

	// Cast string to stream
	std::istringstream iss(msg);

	// Get first line
	std::string line;
	std::getline(iss, line);
	std::istringstream iss_line(trim(line));
	iss_line >> _method >> _path >> _version;
	// Check if line is valid
	if (iss_line.fail() || !iss_line.eof())
		return (_error = true, this->clear());

	// Get headers
	while (std::getline(iss, line) && line != "\r" && line != "")
	{
		// Check if line is vali
		size_t index = line.find(':');
		if (index == std::string::npos)
			return (_error = true, this->clear());
		// Parse line
		std::string key = trim(line.substr(0, index));
		std::string value = trim(line.substr(index + 1));
		_headers.push_back(std::make_pair(key, value));
	}

	// Get body
	_body = iss.str().substr(iss.tellg());

	_requestReady = true;
}

std::string HttpRequest::to_string() const
{
	if (!_requestReady)
		return "";
	std::string ret = _method + " " + _path + " " + _version + "\r\n";
	size_t header_size = _headers.size();
	for (size_t i = 0; i < header_size; i++)
		ret += _headers[i].first + ": " + _headers[i].second + "\r\n";
	ret += "\r\n" + _body;
	return ret;
}

void HttpRequest::clear()
{
	_method.clear();
	_path.clear();
	_version.clear();
	_headers.clear();
	_body.clear();
	_requestReady = false;
	_headerReady = false;
	_error = false;
}

bool HttpRequest::empty() const
{
	return (_method.empty()
		&& _path.empty()
		&& _version.empty()
		&& _headers.empty()
		&& _body.empty());
}

bool HttpRequest::requestReady() const
{
	return _requestReady;
}

bool HttpRequest::error() const
{
	return _error;
}

HttpRequest & HttpRequest::operator=(HttpRequest const & rhs)
{
	if (this != &rhs)
	{
		this->_method = rhs._method;
		this->_path = rhs._path;
		this->_version = rhs._version;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
		this->_requestReady = rhs._requestReady;
		this->_headerReady = rhs._headerReady;
		this->_error = rhs._error;
		this->_buffer = rhs._buffer;
	}
	return *this;
}

std::string HttpRequest::operator()() const
{
	return to_string();
}

std::vector<std::string> HttpRequest::operator[](const std::string& key) const
{
	return getHeader(key);
}

int HttpRequest::parseHeader(const std::string& header)
{
	_headers.clear();

	std::istringstream iss(header);

	// Get first line
	std::string line;
	std::getline(iss, line);
	std::istringstream iss_line(trim(line));
	iss_line >> _method >> _path >> _version;
	// Check if line is valid
	if (iss_line.fail() || !iss_line.eof())
		return (_method.clear(), _path.clear(), _version.clear(), _headers.clear(), 1);

	// Get headers
	while (std::getline(iss, line) && line != "\r" && line != "")
	{
		// Check if line is vali
		size_t index = line.find(':');
		if (index == std::string::npos)
			return (_method.clear(), _path.clear(), _version.clear(), _headers.clear(), 1);
		// Parse line
		std::string key = trim(line.substr(0, index));
		std::string value = trim(line.substr(index + 1));
		_headers.push_back(std::make_pair(key, value));
	}

	return 0;
}

std::ostream & operator<<(std::ostream & o, HttpRequest const & rhs)
{
	if (rhs._requestReady)
	{
		o << rhs.to_string();
		return o;
	}
	o << CYAN << "_method: " << RESET << rhs._method << std::endl;
	o << CYAN << "_path: " << RESET << rhs._path << std::endl;
	o << CYAN << "_version: " << RESET << rhs._version << std::endl;
	o << CYAN << "_headers: " << RESET << rhs._headers.size() << std::endl;
	for (size_t i = 0; i < rhs._headers.size(); i++)
		o << rhs._headers[i].first << ": " << rhs._headers[i].second << std::endl;
	o << CYAN << "_body: " << RESET << rhs._body << std::endl;
	o << CYAN << "_buffer: " << RESET << rhs._buffer << std::endl;

	o << CYAN << "_requestReady: " << RESET << rhs._requestReady << std::endl;
	o << CYAN << "_headerReady: " << RESET << rhs._headerReady << std::endl;
	o << CYAN << "_error: " << RESET << rhs._error << std::endl;
	return o;
}