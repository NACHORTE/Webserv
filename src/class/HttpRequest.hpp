#pragma once
#include <string>
#include <vector>
#include <set>

/*	HttpRequest
 *	Represents an HTTP request
 *	Contains the following fields:
 *		- method: GET, POST, PUT, DELETE, HEAD, CONNECT, OPTIONS, TRACE, PATCH
 *		- query string: key=value&key=value...
 *		- path: /path/to/file
 *		- headers: key-value pairs
 *		- body: body of the request
*/
class HttpRequest {
	public:
	// constructors, destructor, copy constructor

		HttpRequest();
		HttpRequest(const std::string& request);
		HttpRequest(HttpRequest const & src);
		~HttpRequest();

	// getters, setters

		void setMethod(const std::string& method);
		const std::string & getMethod() const;

		void setPath(const std::string& path);
		const std::string & getPath() const;
		
		const std::string & getQueryString() const;

		void setHeader(const std::string& key, const std::string& value);
		void unsetHeader(const std::string& key);
		std::vector<std::string> getHeader(const std::string& key) const;
		const std::vector<std::pair<std::string, std::string> > & getHeaders() const;

		void setBody(const std::string& body, bool chunked = false);
		const std::string & getBody() const;

		void requestReady(bool ready);
		bool requestReady() const;

	// member functions

		long long int addData(const std::string & data);
		std::string to_string() const;
		void clear();
		bool error() const;

	// operator overloads

		HttpRequest & operator=(HttpRequest const & rhs);

	protected:
	private:
	// Member attributes

		// HTTP Method (GET, POST, PUT, DELETE, HEAD, CONNECT, OPTIONS, TRACE, PATCH)
		std::string _method;
		// Path to the file with no query string
		std::string _path;
		// Query string (key=value&key=value...)
		std::string _queryString;
		// Http headers (key-value pairs), can have multiple headers with the same key
		std::vector<std::pair<std::string, std::string> > _headers;	// key-value pairs
		// Body of the request (can be empty) (Content-Length must be set in the headers)
		std::string _body;
	
		// The request is complete so it can be processed
		bool _requestReady;
		// The header has been parsed (used on addData)
		bool _headerReady;
		// The request is in an error state
		bool _error;
		// Buffer used while adding data to the request (used on addData)
		std::string _inBuff;

	// Private member functions

		int parseHeader(const std::string& header);

	// Friends <3
		friend std::ostream & operator<<(std::ostream & o, HttpRequest const & rhs);
};

std::ostream & operator<<(std::ostream & o, HttpRequest const & rhs); 
