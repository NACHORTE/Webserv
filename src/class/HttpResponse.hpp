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
		HttpResponse(
			HttpRequest const & req,
			const Locations & valid_paths,
			const std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)> & valid_methods);
		~HttpResponse();

	// getters, setters
		void setStatus(int code, const std::string& phrase = "");
		void setHeader(const std::string& key, const std::string& value);
		void unsetHeader(const std::string& key);
		void setBody(const std::string& content_type, const std::string& body);
		void setBody(const std::string& filename);

		const std::string & getStatusCode() const;
		const std::string & getStatusPhrase() const;
		std::vector<std::string> getHeader(const std::string& key) const;
		const std::string & getBody() const;

	// member functions
		void clear();
		bool empty() const;
		std::string to_string() const;
		void generate(
			const HttpRequest & req,
			const Locations & valid_paths,
			const std::map<std::string, HttpResponse (*)(const HttpRequest &, const Locations &)> & valid_methods);
		static HttpResponse error(
			int code,
			const std::string & phrase = "",
			const std::string & msg = "");
		bool responseReady() const;
		std::string popResponse(size_t length = -1);
	// operator overloads
		HttpResponse & operator=(HttpResponse const & rhs);
		std::string operator()() const;

	protected:
	private:
	// Member attributes
		std::string _status_code;
		std::string _status_phrase;
		std::vector<std::pair<std::string, std::string> > _headers;
		std::string _body;

		std::set<std::string> _flags;
		std::string _responseBuffer;
		bool _responseReady;

	// Private member functions

	// Friends <3
		friend std::ostream & operator<<(std::ostream & o, HttpResponse const & rhs);
};

std::ostream & operator<<(std::ostream & o, HttpResponse const & rhs);
