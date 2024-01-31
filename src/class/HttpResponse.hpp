#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "HttpRequest.hpp"
#include "Locations.hpp"
#include <map>

class HttpResponse {
	public:
		// Constructor, destructor, copy constructor
		HttpResponse();
		HttpResponse(HttpResponse const & src);
		HttpResponse( // Executes generate response
			HttpRequest const & req,
			const Locations & valid_paths,
			const std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)> & valid_methods);
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
		void clear(); // clear the response
		bool empty() const; // check if the response is empty
		std::string to_string() const; // convert an HttpResponse to a string
		void generate( // Generate a response based on the request (and allowed paths and methods)
			const HttpRequest & req,
			const Locations & valid_paths,
			const std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)> & valid_methods);
		static HttpResponse Error(	// generate an error response
			int code,
			const std::string & phrase = "",
			const std::string & msg = "");
	
		// operator overloads
		HttpResponse & operator=(HttpResponse const & rhs);
		std::string operator()() const; // equivalent to to_string()
	protected:
	private:
		std::string _status_code;
		std::string _status_phrase;
		std::vector<std::pair<std::string, std::string> > _headers;
		std::string _body;

		static void initErrorPages(); // initialize the error pages map
		static std::map<int, std::string> _errorPages; // map of error codes to error files (for Error)

	friend std::ostream & operator<<(std::ostream & o, HttpResponse const & rhs);
};

std::ostream & operator<<(std::ostream & o, HttpResponse const & rhs);
