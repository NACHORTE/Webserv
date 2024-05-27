#pragma once
#include <iostream>
#include <set>

class Location
{
	public:
	// Constructors and destructor

		Location(void);
		Location(const Location & src);
		Location(const std::string & URI);
		~Location();

	// Setters and getters
		
	// Member functions

		bool isPathAllowed(const std::string & path) const;
		bool isMethodAllowed(const std::string & method) const;

	// Operator overloads
		Location & operator=(const Location & rhs);
		bool operator==(const Location & rhs) const;
		bool operator==(const std::string & rhs) const;
		bool operator<(const Location & rhs) const;

	// Public attributes

		std::string _URI;
		std::string _filename;
		std::string _index;
		std::string _root;
		bool _isFile;
		std::set<std::string> _allowedMethods;
		bool _isCgi;
		bool _autoIndex;

	protected:
	private:
	// Private attributes

	// Private member functions

	// Friends <3
		friend std::ostream &operator<<(std::ostream &os, const Location &obj);
};

std::ostream &operator<<(std::ostream &os, const Location &obj);