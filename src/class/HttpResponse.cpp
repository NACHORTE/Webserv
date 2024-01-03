#include "HttpResponse.hpp"
#include <sstream>
#include "utils.hpp"
#include <map>

HttpResponse::HttpResponse()
{
	this->status = "HTTP/1.1 200 OK\n";
	this->content_type = "Content-Type: text/plain\n";
	this->content_len = "Content-Length: 29\n\n";
	this->body = "Default body";
}

HttpResponse::HttpResponse(const std::string& status, const std::string& contentType,
	int content_len, const std::string& body)
{
	this->status = "HTTP/1.1 " + status + "\r\n";
	this->content_type = "Content-Type: " + contentType + "\r\n";
	this->content_len = "Content-Length: " + int_to_string(content_len) + "\r\n\r\n";
	this->body = body;
}

HttpResponse::~HttpResponse()
{}

void HttpResponse::set_status(int code)
{
	std::map<int, std::string>::iterator it;
	std::map<int, std::string> status_codes =
	{
		{200, "OK"},
        {201, "Created"},
        {204, "No Content"},
        {400, "Bad Request"},
        {401, "Unauthorized"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {500, "Internal Server Error"}
	};
	
	this->status = "HTTP/1.1 " + int_to_string(code);
	it = status_codes.find(code);
	if (it != status_codes.end())
		this->status += " " + it->second + "\r\n";
	else if (code >= 200 && code < 300)
		this->status += " OK\r\n";
	else if (code >=400 && code < 500)
		this->status += " Not Found\r\n";
	else if (code >= 500)
		this->status += " Internal Server Error\r\n";
	else
		this->status += " Unknown Status\r\n";
}

void HttpResponse::set_body(const std::string& content_type, const std::string& body)
{
	this->content_type = "Content-Type: " + content_type + "\r\n";
	this->body = body;
	this->content_len = "Content-Length: " + int_to_string(body.length()) + "\n\n";
}

int HttpResponse::get_length()
{
	return this->body.length();
}

std::string HttpResponse::get_response()
{
	return this->status + this->content_type + this->content_len + this->body;
}