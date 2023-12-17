#pragma once
#include <string>

class HttpResponse {
	public:
	HttpResponse();
	HttpResponse(const std::string& status, const std::string& contentType, int content_len, const std::string& body);
	~HttpResponse();

	void set_status(int code);
	void set_body(const std::string& content_type, const std::string& body);
	int get_length();
	std::string get_response();
	private:
	std::string status;
	std::string content_type;
	std::string content_len;
	std::string body;
};