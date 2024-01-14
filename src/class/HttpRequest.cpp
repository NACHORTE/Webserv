#include "HttpRequest.hpp"
#include <sstream>
#include "utils.hpp"

HttpRequest::HttpRequest()
{}

HttpRequest::HttpRequest(const std::string& msg)
{
	parseRequest(msg);
}

HttpRequest::HttpRequest(const HttpRequest& other):
	method(other.method), path(other.path), version(other.version),
	headers(other.headers), body(other.body)
{}

HttpRequest::~HttpRequest()
{}

void HttpRequest::set_method(const std::string& method)
{
	this->method = method;
}

void HttpRequest::set_path(const std::string& path)
{
	this->path = path;
}

void HttpRequest::set_version(const std::string& version)
{
	this->version = version;
}

void HttpRequest::set_header(const std::string& key, const std::string& value)
{
	// Multiple headers with the same key are allowed
	headers.push_back(std::make_pair(key, value));
}

void HttpRequest::unset_header(const std::string& key)
{
	std::vector<std::pair<std::string, std::string> >::iterator it;
	for (it = headers.begin(); it != headers.end();)
	{
		if (it->first == key)
			it = headers.erase(it);
		else
			++it;
	}
}

void HttpRequest::set_body(const std::string& body)
{
	this->body = body;
}

const std::string & HttpRequest::get_method() const
{
	return method;
}

const std::string & HttpRequest::get_path() const
{
	return path;
}

const std::string & HttpRequest::get_version() const
{
	return version;
}

std::vector<std::string> HttpRequest::get_header(const std::string& key) const
{
	std::vector<std::string> output;

	size_t header_size = headers.size();
	for (size_t i = 0; i < header_size; i++)
		if (headers[i].first == key)
			output.push_back(headers[i].second);
	return output;
}

const std::string & HttpRequest::get_body() const
{
	return body;
}

void HttpRequest::parseRequest(const std::string& msg)
{
	// Clear current request
	this->clear();

	// Cast string to stream
	std::istringstream iss(msg);

	// Get first line
	std::string line;
	std::getline(iss, line);
	std::istringstream iss_line(line);
	iss_line >> method >> path >> version;
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
		headers.push_back(std::make_pair(key, value));
	}

	// Get body
	body = iss.str().substr(iss.tellg());
}

std::string HttpRequest::to_string() const
{
	if (this->empty())
		return "";
	std::string ret = method + " " + path + " " + version + "\r\n";
	size_t header_size = headers.size();
	for (size_t i = 0; i < header_size; i++)
		ret += headers[i].first + ": " + headers[i].second + "\r\n";
	ret += "\r\n" + body;
	return ret;
}

void HttpRequest::clear()
{
	method.clear();
	path.clear();
	version.clear();
	headers.clear();
	body.clear();
}

bool HttpRequest::empty() const
{
	return (method.empty()
		&& path.empty()
		&& version.empty()
		&& headers.empty()
		&& body.empty());
}

HttpRequest & HttpRequest::operator=(HttpRequest const & rhs)
{
	if (this != &rhs)
	{
		this->method = rhs.method;
		this->path = rhs.path;
		this->version = rhs.version;
		this->headers = rhs.headers;
		this->body = rhs.body;
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