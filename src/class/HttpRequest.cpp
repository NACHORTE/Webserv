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

void HttpRequest::setMethod(const std::string& method)
{
	this->_method = method;
}

void HttpRequest::setPath(const std::string& path)
{
	this->_path = path;
}

void HttpRequest::setVersion(const std::string& version)
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

long long int getChunk(std::string & buf, std::string & body)
{
	//get the length of the chunk
	size_t chunkSize;
	size_t pos = buf.find("\r\n");
	if (pos == std::string::npos)
		return -1;
	std::istringstream iss(buf.substr(0, pos));
	iss >> std::hex >> chunkSize;
	if (iss.fail() or not iss.eof())
		return -1;

	// if the buffer is not long enough, return error
	// hex size + \r\n + chunkSize + \r\n
	if (pos + chunkSize + 4 > buf.size())
		return -1;
	
	// add the chunk to the body and remove it from the buffer
	body += buf.substr(pos + 2, chunkSize);
	buf = buf.substr(pos + chunkSize + 4);

	// return the read size
	return pos + chunkSize + 4;
}

long long int HttpRequest::addData(const std::string & data)
{
	if (_requestReady)
		return 0;

	_buffer = _buffer + data;
	// If the header has not been found yet, search for it
	if (not _headerReady)
	{
		// Find the end of the header
		size_t pos = _buffer.find("\r\n\r\n");
		// If the end of the header is not found, return the number of bytes read
		if (pos == std::string::npos)
			return data.size();
		// If the end of the header is found, parse it
		if (parseHeader(_buffer.substr(0, pos + 4)) == 1)
			return (_error=true, -1); //TODO 400 error
		_buffer = _buffer.substr(pos + 4);
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
					return (_error=true, -1); //TODO 500 error
			}
			// If no content length is found, or some weird error occurs, return
			if (contentLength < _body.size())
				return (_error=true, -1); // TODO 400 error
			// Add the data to the body
			size_t len = (contentLength - _body.size() > _buffer.size()) ?
				_buffer.size() : contentLength - _body.size();
			_body += _buffer.substr(0, len);
			_buffer = _buffer.substr(len);

			// If the body is complete, set the request as ready
			if (_body.size() == contentLength)
				_requestReady = true;
		}
		//is chunked
		else
		{
 			try
			{
				long long int chunkSize;
				do
				{
					chunkSize=getChunk(_buffer, _body);
					// 0\r\n\r\n
					if (chunkSize == 5)
					{
						_requestReady = true;
						unsetHeader("Transfer-Encoding");
						setHeader("Content-Length", intToString(_body.size()));
					}
				} while (chunkSize > 5);
			}
			catch(const std::exception& e)
			{
				return (_error=true, -1); //TODO 400 error
			}
		}
	}

	return data.size() - _buffer.size(); 
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
#include <iostream> //XXX
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