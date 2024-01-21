#include "HttpResponse.hpp"
#include <sstream>
#include "utils.hpp"
#include <map>

HttpResponse::HttpResponse()
{}

HttpResponse::HttpResponse(const HttpResponse& other):
	status_code(other.status_code),
	status_phrase(other.status_phrase),
	headers(other.headers),
	body(other.body)
{}

HttpResponse::HttpResponse(const HttpRequest& req, const Locations & valid_paths)
{
	generate_response(req, valid_paths);
}

HttpResponse::~HttpResponse()
{}


void HttpResponse::set_status(int code, const std::string& phrase)
{
	this->status_code = int_to_string(code);
	if (phrase.length() > 0)
		this->status_phrase = phrase;
	else
	{
		if (code >= 200 && code < 300)
			this->status_phrase = "OK";
		else if (code >=400 && code < 500)
			this->status_phrase = "Not Found";
		else if (code >= 500)
			this->status_phrase = "Internal Server Error";
		else
			this->status_phrase = "";
	}
}

void HttpResponse::set_header(const std::string& key, const std::string& value)
{
	// Multiple headers with the same key are allowed
	headers.push_back(std::make_pair(key, value));
}

void HttpResponse::unset_header(const std::string& key)
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

void HttpResponse::set_body(const std::string& content_type, const std::string& body)
{
	this->body = body;
	unset_header("Content-Type");
	unset_header("Content-Length");
	if (body.length() > 0)
	{
		set_header("Content-Type", content_type);
		set_header("Content-Length", int_to_string(body.length()));
	}
}

const std::string & HttpResponse::get_status_code() const
{
	return status_code;
}

const std::string & HttpResponse::get_status_phrase() const
{
	return status_phrase;
}

std::vector<std::string> HttpResponse::get_header(const std::string& key) const
{
	std::vector<std::string> ret;
	std::vector<std::pair<std::string, std::string> >::const_iterator it;
	for (it = headers.begin(); it != headers.end(); ++it)
	{
		if (it->first == key)
			ret.push_back(it->second);
	}
	return ret;
}

const std::string & HttpResponse::get_body() const
{
	return body;
}

void HttpResponse::clear()
{
	status_code.clear();
	status_phrase.clear();
	headers.clear();
	body.clear();
}

bool HttpResponse::empty() const
{
	return (status_code.empty()
		&& status_phrase.empty()
		&& headers.empty()
		&& body.empty());
}

std::string HttpResponse::to_string() const
{
	if (status_code == "")
		return "";

	std::string output;
	output += "HTTP/1.1 " + status_code + " " + status_phrase + "\r\n";
	size_t header_size = headers.size();
	for (size_t i = 0; i < header_size; i++)
		output += headers[i].first + ": " + headers[i].second + "\r\n";
	output += "\r\n" + body;
	return output;
}

static bool isBinaryFile(const std::string & filename)
{
	std::string contentType = getContentType(filename);
	return (contentType.substr(0,5) != "text/");
}

void HttpResponse::generate_response(const HttpRequest& req, const Locations & valid_paths)
{
	// Empty everything
	clear();

	// If request is not valid, return 400 Bad Request
	if (valid_paths.isPathAllowed(req.get_method(), req.get_path()) == false)
	{
		//NOTE Maybe try a file and if it fails, return this
		set_status(403);
		set_body("text/html", "<html><body><h1>403 Forbidden</h1></body></html>");
		return;
	}

	//THIS SHOULD GO IN THE GET METHOD FROM THE SERVER
	// Get the path of the requested file
	std::string filename = valid_paths.getFilename(req.get_method(), req.get_path());
	std::string extension = getExtension(filename);

	// If the file ends with ".cgi", run the file and return the output
	if (extension == "cgi")
	{
		//something with fork and execve
		return;
	}

	// The request was succesful
	try
	{
		set_status(200);
		set_body(getContentType(filename), readFile(filename, isBinaryFile(filename)));
	}
	// Catch FileNotFound and return 404 Not Found
	catch (FileNotFound & e)
	{
		// NOTE Maybe try a file and if it fails, return this
		set_status(404);
		set_body("text/html", "<html><body><h1>404 Not Found</h1></body></html>");
		return;
	}
	// Catch FileNotOpen and any other exceptions
	catch (std::exception & e)
	{
		// NOTE Maybe try a file and if it fails, return this
		set_status(500);
		set_body("text/html", "<html><body><h1>500 Internal Server Error</h1></body></html>");
		return;
	}
}

HttpResponse & HttpResponse::operator=(const HttpResponse& rhs)
{
	if (this != &rhs)
	{
		this->status_code = rhs.status_code;
		this->status_phrase = rhs.status_phrase;
		this->headers = rhs.headers;
		this->body = rhs.body;
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