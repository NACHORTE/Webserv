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

HttpResponse::HttpResponse(const HttpRequest& req, const std::vector<std::string>& allowed_paths)
{
	generate_response(req, allowed_paths);
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

static bool allowed_path(std::string path, std::vector<std::string> allowed_paths) {
	// path "/" is always allowed
	if (path == "/")
		return true;

	// if path contains ".." it is not allowed
	if (path.find("..") != std::string::npos)
		return false;

	// search for path starting with allowed paths
	std::vector<std::string>::iterator it;
	for (it = allowed_paths.begin(); it != allowed_paths.end(); it++)
	{
		// compare path with iterator as a file
		if (back(*it) != '/'	// if allowed path does not end with "/" its a file
			&& path == *it)	// path must be exactly the same as allowed path
			return true;

		// compare path with iterator as a directory
		if (back(*it) == '/'	// if allowed path ends with "/" its a directory
			&& path.compare(0, it->size(), *it) == 0	// if path starts with allowed path
			&& path.size() > it->size())	// if path is longer than allowed path (is a file in the directory)
			return true;
	}

	return false;
}

std::string get_content_type(const std::string & path) {
	std::vector<std::pair<std::string, std::string> > content_type;
	content_type.push_back(std::make_pair(".html", "text/html"));
	content_type.push_back(std::make_pair(".css", "text/css"));
	content_type.push_back(std::make_pair(".js", "application/javascript"));
	content_type.push_back(std::make_pair(".jpg", "image/jpeg"));
	content_type.push_back(std::make_pair(".jpeg", "image/jpeg"));
	content_type.push_back(std::make_pair(".png", "image/png"));
	content_type.push_back(std::make_pair(".gif", "image/gif"));
	content_type.push_back(std::make_pair(".swf", "application/x-shockwave-flash"));
	content_type.push_back(std::make_pair(".txt", "text/plain"));
	content_type.push_back(std::make_pair(".pdf", "application/pdf"));
	content_type.push_back(std::make_pair(".mp3", "audio/mpeg"));
	content_type.push_back(std::make_pair(".mp4", "video/mp4"));
	content_type.push_back(std::make_pair(".avi", "video/x-msvideo"));
	content_type.push_back(std::make_pair(".mpeg", "video/mpeg"));
	content_type.push_back(std::make_pair(".ico", "image/x-icon"));

	std::string extension = path.substr(path.find_last_of('.'));
	if (extension == "")
		return "text/html";
	for (size_t i = 0; i < content_type.size(); i++)
		if (content_type[i].first == extension)
			return content_type[i].second;
	return "text/html";
}

void HttpResponse::generate_response(const HttpRequest& req, const std::vector<std::string>& allowed_paths)
{
	// clear the response
	clear();

	// check if path is allowed
	if (!allowed_path(req.get_path(), allowed_paths))
	{
		set_status(403, "Forbidden");
		return;
	}

	// check if method is allowed
	if (req.get_method() != "GET")
	{
		set_status(405, "Method Not Allowed");
		return;
	}
	if (req.get_path() == "/" || req.get_path() == "/index.html")
	{
		set_status(200, "OK");
		set_body("text/html", readFile("./html/index.html"));
		return;
	}
	// check if file exists
	std::string file_path = "." + req.get_path();
	std::string file_content;
	try
	{
		file_content = readImageFile(file_path);
	}
	catch (const std::invalid_argument& e)
	{
		set_status(404, "Not Found");
		return;
	}
	catch (const std::runtime_error& e)
	{
		set_status(500, "Internal Server Error");
		return;
	}

	// set status code
	set_status(200, "OK");
	std::cout << get_content_type(file_path) << std::endl;

	// set body
	set_body(get_content_type(file_path), file_content);
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