#pragma once
#include <iostream>
#include <set>

class Location
{
	public:
	// Constructors and destructor

		Location(void);
		Location(const Location & src);
		~Location();

	// Setters and getters
		
	// Member functions

		// Check if the URI matches this location
		bool matchesURI(const std::string & URI) const;
		// Gets the file associated with the http path for this location
		std::string getFilename(const std::string & path) const;
		// Check if the method is allowed for this location
		bool isMethodAllowed(const std::string & method) const;
		// Creates the response with the html of the content of the folder if there is no index file
		//TODOHttpResponse getAutoIndexResponse(const std::string & path) const;

	// Operator overloads
		Location & operator=(const Location & rhs);
		bool operator==(const Location & rhs) const;
		bool operator==(const std::string & rhs) const;

	// Public attributes

		// If the URI ends with a slash, it's a folder
		std::string _URI;
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

	protected:
	private:
	// Private attributes

	// Private member functions

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Location &obj);
};

std::ostream &operator<<(std::ostream &os, const Location &obj);