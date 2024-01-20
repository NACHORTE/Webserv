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

HttpResponse::HttpResponse(const HttpRequest& req)
{
	generate_response(req);
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

void HttpResponse::generate_response(const HttpRequest& req)
{

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