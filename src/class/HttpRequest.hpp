#pragma once
#include <string>
#include <vector>
#include <set>

/*	HttpRequest
 *	Represents an HTTP request
 *	Contains the following fields:
 *		- method: GET, POST, PUT, DELETE, HEAD, CONNECT, OPTIONS, TRACE, PATCH
 *		- path: /path/to/file
 *		- version: HTTP/1.1
 *		- headers: key-value pairs
 *		- body: body of the request
*/
class HttpRequest {
	public:
		// constructors, destructor, copy constructor

		HttpRequest();
		HttpRequest(const std::string& msg);
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
		std::vector<std::string> getHeader(const std::string& key) const; // returns a vector of all headers with the given key
		const std::vector<std::pair<std::string, std::string> > & getHeaders() const;

		void setBody(const std::string& body, bool chunked = false);
		const std::string & getBody() const;

		void requestReady(bool ready);
		bool requestReady() const;
		// member functions

		long long int addData(const std::string & data);
		std::string to_string() const; // convert an HttpRequest to a string
		void clear(); // clear all fields
		bool error() const; // check if the request is in an error state

		// operator overloads

		HttpRequest & operator=(HttpRequest const & rhs);

	protected:
	private:
	// Member attributes

		std::string _method;	// GET, POST, PUT, DELETE, HEAD, CONNECT, OPTIONS, TRACE, PATCH
		std::string _path;	// /path/to/file
		std::string _queryString;	// key=value&key=value...
		std::vector<std::pair<std::string, std::string> > _headers;	// key-value pairs
		std::string _body;	// body of the request
	
		bool _requestReady;
		bool _headerReady;
		bool _error;

		std::string _buffer; // Used while adding data to the request

	// Private member functions

		int parseHeader(const std::string& header);

	// Friends <3
		friend std::ostream & operator<<(std::ostream & o, HttpRequest const & rhs);
};

std::ostream & operator<<(std::ostream & o, HttpRequest const & rhs); 
