#pragma once
#include <string>
#include <vector>
#include <iostream>

class HttpResponse {
	public:
	// Constructors, destructor, copy constructor

		HttpResponse();
		HttpResponse(const HttpResponse & src);
        HttpResponse(const std::string & response);
		~HttpResponse();

	// getters, setters

		void setStatus(int code, const std::string& phrase = "");
		const std::string & getStatusCode() const;
		const std::string & getStatusPhrase() const;

		void setHeader(const std::string& key, const std::string& value);
		void unsetHeader(const std::string& key);
		std::vector<std::string> getHeader(const std::string& key) const;
		const std::vector<std::pair<std::string, std::string> > & getHeaders() const;

		void setBody(const std::string& body, bool chunked = false);
		const std::string & getBody() const;

		void responseReady(bool ready);
		bool responseReady() const;

	// member functions

		long long int addData(const std::string & data);
		std::string to_string() const;
		void clear();
		bool error() const;
		static HttpResponse errorResponse(int code, const std::string & phrase = "", std::string msg = "");

	// operator overloads

		HttpResponse & operator=(HttpResponse const & rhs);

	protected:
	private:
	// Member attributes

		// HTTP Status code (200, 404, 500, etc.)
		std::string _statusCode;
		// HTTP Status phrase (OK, Not Found, Internal Server Error, etc.) (optional)
		std::string _statusPhrase;
		// HTTP Headers
		std::vector<std::pair<std::string, std::string> > _headers;
		// HTTP Body (optional) (header Content-Type is required)
		std::string _body;

		// Flag to indicate if the response is ready to be sent
		bool _responseReady;
		// The header has been parsed (used on addData)
		bool _headerReady;
		// Flag to indicate if the response has errors while being built
		bool _error;
		// Buffer used while adding data to the request (used on addData)
		std::string _inBuff;

	// Private member functions

		int parseHeader(const std::string& header);

	// Friends <3
		friend std::ostream & operator<<(std::ostream & o, HttpResponse const & rhs);
};

std::ostream & operator<<(std::ostream & o, HttpResponse const & rhs);
