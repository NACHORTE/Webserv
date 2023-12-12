#include "response.hpp"

HttpResponse::HttpResponse()
{
	this->status = "HTTP/1.1 200 OK\n";
	this->content_type = "Content-Type: text/plain\n";
	this->content_len = "Content-Length: 29\n\n";
	this->body = "Default body";
}

HttpResponse::HttpResponse(const std::string& status, const std::string& contentType, int content_len, const std::string& body)
{
	this->status = "HTTP/1.1 " + status + "\n";
	this->content_type = "Content-Type: " + contentType + "\n";
	this->content_len = "Content-Length: " + std::to_string(content_len) + "\n\n";
	this->body = body;
}

HttpResponse::~HttpResponse()
{
}

void HttpResponse::set_status(int code)
{
	this->status = "HTTP/1.1 " + std::to_string(code);
	if (code >= 200 && code < 300)
		this->status += " OK\r\n";
	else if (code >=400 && code < 500)
		this->status += " Not Found\r\n";
	else if (code >= 500)
		this->status += " Internal Server Error\r\n";
	else
		this->status += " Unknown Error\r\n";
}

void HttpResponse::set_content_type(const std::string& content_type)
{
	this->content_type = "Content-Type: " + content_type + "\r\n";
}

void HttpResponse::set_content_len(int content_len)
{
	this->content_len = "Content-Length: " + std::to_string(content_len) + "\n\n";
}

void HttpResponse::set_body(const std::string& body)
{
	this->body = body;
}

int HttpResponse::get_length()
{
	return this->body.length();
}

std::string HttpResponse::get_response()
{
	return this->status + this->content_type + this->content_len + this->body;
}