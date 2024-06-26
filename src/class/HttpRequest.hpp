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

		HttpRequest();	// default constructor sets all fields to empty
		HttpRequest(const std::string& msg); // parse a string into an HttpRequest
		HttpRequest(HttpRequest const & src); // copy constructor
		~HttpRequest(); // destructor does nothing

		// getters, setters

		void setMethod(const std::string& method); //TODO change name format to setMethod
		void setPath(const std::string& path);
		void setVersion(const std::string& version);
		void setHeader(const std::string& key, const std::string& value);
		void unsetHeader(const std::string& key);
		void setBody(const std::string& body);

		// Add data to the request buffer and return the number of bytes added
		long long int addData(const std::string & data);

		const std::string & getMethod() const;
		const std::string & getPath() const;
		const std::string & getVersion() const;
		std::vector<std::string> getHeader(const std::string& key) const; // returns a vector of all headers with the given key
		std::vector<std::pair<std::string, std::string> > getHeaders() const;
		const std::string & getBody() const;

		// member functions

		void parse(const std::string& msg); // parse a string into an HttpRequest
		std::string to_string() const; // convert an HttpRequest to a string
		void clear(); // clear all fields
		bool empty() const; // check if all fields are empty
		bool requestReady() const; // check if the request is complete
		bool error() const; // check if the request is in an error state

		// operator overloads

		HttpRequest & operator=(HttpRequest const & rhs);
		std::string operator()() const; // equivalent to to_string()
		std::vector<std::string> operator[](const std::string& key) const; // equivalent to getHeader(key)

	protected:
	private:
	// Member attributes

		std::string _method;	// GET, POST, PUT, DELETE, HEAD, CONNECT, OPTIONS, TRACE, PATCH
		std::string _path;	// /path/to/file
		std::string _version;	// HTTP/1.1
		std::vector<std::pair<std::string, std::string> > _headers;	// key-value pairs
		std::string _body;	// body of the request
	
		bool _requestReady;
		bool _headerReady;
		bool _error;

		std::string _buffer;

	// Private member functions

		int parseHeader(const std::string& header);

	// Friends <3
		friend std::ostream & operator<<(std::ostream & o, HttpRequest const & rhs);
};

// prints the request using to_string and trimming the body if it is too long
std::ostream & operator<<(std::ostream & o, HttpRequest const & rhs); 
