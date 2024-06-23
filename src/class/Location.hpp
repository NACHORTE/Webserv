#pragma once
#include <iostream>
#include <set>
#include "HttpResponse.hpp"

class Location
{
	public:
	// Constructors and destructor

		Location(const std::string & URI = "");
		Location(const Location & src);
		~Location();

	// Setters and getters
		
		void setURI(const std::string & URI);
		const std::string & getURI(void) const;

		const std::string & getExtension(void) const;

		void setIndex(const std::string & index);
		const std::string & getIndex(void) const;

		void setRoot(const std::string & root);
		const std::string & getRoot(void) const;

		void setAlias(const std::string & alias);
		const std::string & getAlias(void) const;

		void setAllowedMethods(const std::set<std::string> & allowedMethods);
		void addAllowedMethod(const std::string & method);
		const std::set<std::string> & getAllowedMethods(void) const;
		bool isAllowedMethod(const std::string & method) const;

		void isCgi(bool isCgi); // setter
		bool isCgi(void) const; // getter

		void autoIndex(bool autoIndex); // setter
		bool autoIndex(void) const; // getter

		void setReturnValue(int code, const std::string & body);
		const std::pair<int, std::string> & getReturnValue(void) const;
		bool hasReturnValue(void) const;
		// Creates the response with the _returnValue code and string (if unset returns 500)
		HttpResponse getReturnResponse(void) const;

	// Member functions

		// Check if the location is for a folder
		bool isDir(std::string path) const;
		// Check if the URI matches this location
		bool matchesURI(std::string path) const;
		// Gets the file associated with the http path for this location
		std::string getFilename(std::string path) const;
		std::string getFilenameNoIndex(std::string path) const;
		// Clears the location NOTE remove it
		void clear(void);

	// Operator overloads

		Location & operator=(const Location & rhs);
		bool operator==(const Location & rhs) const;
		bool operator==(const std::string & rhs) const;

	// Public attributes


	protected:
	private:
	// Private attributes

		// If the URI ends with a slash, it's a folder
		std::string _URI;
		// Matches the extension of the file (*.log *.html *.css ...)
		std::string _extension;
		// Index is the file that will be served if the URI is a folder (default is index.html)
		std::string _index;
		// Root is from where the class will serve the files (root + path)
		std::string _root;
		// Alias replaces the URI of the request when serving the file (it can switch to a file or a folder)
		std::string _alias;
		// Allowed methods for this location. If empty, all methods are allowed
		std::set<std::string> _allowedMethods;
		// If the location is a cgi script
		bool _isCgi;
		// If the location is a folder and doesn't have an index file
		// autoindex on will generate an html with the ls of files 
		bool _autoIndex;
		// If _returnValue it's set, any match to this path will return only an http with the code and string
		// GET /thispath -> httpresponse = _returnValue.first, body=_returnValue.second
		// returnValue.first = -1 means not set
		std::pair<int, std::string> _returnValue;

	// Private member functions

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Location &obj);
};

std::ostream &operator<<(std::ostream &os, const Location &obj);