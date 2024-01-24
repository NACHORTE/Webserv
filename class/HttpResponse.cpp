#include "HttpResponse.hpp"
#include <sstream>
#include "utils.hpp"
#include <map>

HttpResponse::HttpResponse()
{}

HttpResponse::HttpResponse(const HttpResponse& other):
	_status_code(other._status_code),
	_status_phrase(other._status_phrase),
	_headers(other._headers),
	_body(other._body)
{}

HttpResponse::HttpResponse(
	const HttpRequest& req,
	const Locations & valid_paths,
	const std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)> & valid_methods)
{
	generate(req, valid_paths, valid_methods);
}

HttpResponse::~HttpResponse()
{}


void HttpResponse::set_status(int code, const std::string& phrase)
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

void HttpResponse::set_header(const std::string& key, const std::string& value)
{
	// Multiple headers with the same key are allowed
	_headers.push_back(std::make_pair(key, value));
}

void HttpResponse::unset_header(const std::string& key)
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

void HttpResponse::set_body(const std::string& content_type, const std::string& body)
{
	this->_body = body;
	unset_header("Content-Type");
	unset_header("Content-Length");
	if (_body.length() > 0)
	{
		set_header("Content-Type", content_type);
		set_header("Content-Length", int_to_string(_body.length()));
	}
}

const std::string & HttpResponse::get_status_code() const
{
	return _status_code;
}

const std::string & HttpResponse::get_status_phrase() const
{
	return _status_phrase;
}

std::vector<std::string> HttpResponse::get_header(const std::string& key) const
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

const std::string & HttpResponse::get_body() const
{
	return (_body);
}

void HttpResponse::clear()
{
	_status_code.clear();
	_status_phrase.clear();
	_headers.clear();
	_body.clear();
}

bool HttpResponse::empty() const
{
	return (_status_code.empty()
		&& _status_phrase.empty()
		&& _headers.empty()
		&& _body.empty());
}

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

void HttpResponse::generate(
	const HttpRequest & req,
	const Locations & valid_paths,
	const std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)> & valid_methods)
{
	// Empty everything
	clear();

	// If method is not valid, return 405 Method Not Allowed
	if (valid_methods.count(req.get_method()) == 0)
	{
		// NOTE Maybe try a file and if it fails, return this
		set_status(405, "Method Not Allowed");
		set_body("text/html", "<html><body><h1>405 Method Not Allowed</h1></body></html>");
		return;
	}

	// If request is not valid, return 400 Bad Request
	if (valid_paths.isPathAllowed(req.get_method(), req.get_path()) == false)
	{
		//NOTE Maybe try a file and if it fails, return this
		set_status(403, "Forbidden");
		set_body("text/html", "<html><body><h1>403 Forbidden</h1></body></html>");
		return;
	}

	// Generate response with the appropriate method
	try
	{
		*this = valid_methods.at(req.get_method())(req, valid_paths);
	}
	catch (std::exception & e)
	{
		// NOTE Maybe try a file and if it fails, return this
		set_status(500, "Internal Server Error");
		set_body("text/html", "<html><body><h1>500 Internal Server Error</h1></body></html>");
	}
}

HttpResponse & HttpResponse::operator=(const HttpResponse& rhs)
{
	if (this != &rhs)
	{
		this->_status_code = rhs._status_code;
		this->_status_phrase = rhs._status_phrase;
		this->_headers = rhs._headers;
		this->_body = rhs._body;
	}
	return *this;
}

std::string HttpResponse::operator()() const
{
	return to_string();
}

std::ostream & operator<<(std::ostream & o, const HttpResponse& rhs)
{
	o << rhs.to_string();
	return o;
}