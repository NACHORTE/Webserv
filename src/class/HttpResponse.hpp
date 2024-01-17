#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "HttpRequest.hpp"

class HttpResponse {
	public:
		// constructor, destructor, copy constructor
		HttpResponse();
		HttpResponse(HttpResponse const & src);
		HttpResponse(HttpRequest const & src, const std::vector<std::string> & allowed_paths);
		~HttpResponse();

		// getters, setters
		void set_status(int code, const std::string& phrase = "");
		void set_header(const std::string& key, const std::string& value);
		void unset_header(const std::string& key);
		void set_body(const std::string& content_type, const std::string& body);

		const std::string & get_status_code() const;
		const std::string & get_status_phrase() const;
		std::vector<std::string> get_header(const std::string& key) const; // returns a vector of all headers with the given key
		const std::string & get_body() const;

		// member functions
		void clear();
		bool empty() const;
		std::string to_string() const; // convert an HttpResponse to a string
		void generate_response(const HttpRequest & req, const std::vector<std::string> & allowed_paths); // generate a response based on the request (and allowed paths)

		// operator overloads
		HttpResponse & operator=(HttpResponse const & rhs);
		std::string operator()() const; // equivalent to to_string()
	protected:
	private:
		std::string status_code;
		std::string status_phrase;
		std::vector<std::pair<std::string, std::string> > headers;
		std::string body;
	
	friend std::ostream & operator<<(std::ostream & o, HttpResponse const & rhs);
};

std::ostream & operator<<(std::ostream & o, HttpResponse const & rhs);
