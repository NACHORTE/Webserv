#pragma once
#include <iostream>
#include <vector>

class Location
{
	public:
	// Constructors and destructor

		Location(void);
		Location(const Location & src);
		~Location();

	// Setters and getters

		void setPath(const std::string & path);
		void setRoot(const std::string & root);
		void setIndex(const std::string & index);
		void setAutoindex(const std::string & autoindex);
		void setAllowMethods(const std::vector<std::string> & allowMethods);

		void addAllowMethod(const std::string & allowMethod);

		const std::string & getPath() const;
		const std::string & getRoot() const;
		const std::string & getIndex() const;
		const std::string & getAutoindex() const;
		const std::vector<std::string> & getAllowMethods() const;

	// Member functions

	// Operator overloads

		Location & operator=(const Location & rhs);

	protected:
	private:
	// Member attributes
		// if True, returns a list of files in the directory if no index file is found
		std::string _autoindex;
		// Path to the location
		std::string _uri;
		// Root directory for the location
		std::string _root;
		// Default file to serve when the request is for a directory
		std::string _index;
		// Rename the path of the uri to the alias
		std::string _alias;
		// Return redirect to the client
		std::pair<int, std::string> _return;
		// List of allowed methods for the location
		std::vector<std::string> _allowMethods;

	// Private member functions

	// Friends <3

		friend std::ostream &operator<<(std::ostream &os, const Location &obj);
};

std::ostream &operator<<(std::ostream &os, const Location &obj);
