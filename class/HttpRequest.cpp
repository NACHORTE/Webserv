#include "HttpRequest.hpp"
#include <sstream>
#include "utils.hpp"

HttpRequest::HttpRequest()
{}

HttpRequest::HttpRequest(const std::string& msg)
{
	parse(msg);
}

HttpRequest::HttpRequest(const HttpRequest& other):
	_method(other._method), _path(other._path), _version(other._version),
	_headers(other._headers), _body(other._body)
{}

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

void HttpRequest::set_header(const std::string& key, const std::string& value)
{
	// Multiple headers with the same key are allowed
	_headers.push_back(std::make_pair(key, value));
}

void HttpRequest::unset_header(const std::string& key)
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

void HttpRequest::set_body(const std::string& body)
{
	this->_body = body;
}

const std::string & HttpRequest::get_method() const
{
	return _method;
}

const std::string & HttpRequest::get_path() const
{
	return _path;
}

const std::string & HttpRequest::get_version() const
{
	return _version;
}

std::vector<std::string> HttpRequest::get_header(const std::string& key) const
{
	std::vector<std::string> output;

	size_t header_size = _headers.size();
	for (size_t i = 0; i < header_size; i++)
		if (_headers[i].first == key)
			output.push_back(_headers[i].second);
	return output;
}

const std::string & HttpRequest::get_body() const
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
	std::istringstream iss_line(line);
	iss_line >> _method >> _path >> _version;
	// Check if line is valid
	if (iss_line.fail()) // NOTE check if there is no more words in iss_line
		return (this->clear());

	// Get headers
	while (std::getline(iss, line) && line != "\r" && line != "")
	{
		// Check if line is vali
		size_t index = line.find(':');
		if (index == std::string::npos)
			return (this->clear());
		// Parse line
		std::string key = trim(line.substr(0, index));
		std::string value = trim(line.substr(index + 1));
		_headers.push_back(std::make_pair(key, value));
	}

	// Get body
	_body = iss.str().substr(iss.tellg());
}

std::string HttpRequest::to_string() const
{
	if (this->empty())
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
}

bool HttpRequest::empty() const
{
	return (_method.empty()
		&& _path.empty()
		&& _version.empty()
		&& _headers.empty()
		&& _body.empty());
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
	}
	return *this;
}

std::string HttpRequest::operator()() const
{
	return to_string();
}

std::ostream & operator<<(std::ostream & o, HttpRequest const & rhs)
{
	o << rhs.to_string();
	return o;
}