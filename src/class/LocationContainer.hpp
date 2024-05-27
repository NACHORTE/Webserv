#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include "Location.hpp"

class LocationContainer
{
	public:
	// Constructors and destructor

		LocationContainer(void);
		LocationContainer(const LocationContainer & src);
		~LocationContainer();

	// Setters and getters

		std::string getFilename(const std::string & path) const; // returns the filename associated with the path. Returns empty string if no match is found

	// Member functions

		bool addLocation(const Location & location);
		bool addLocation(std::string path, bool isFile, std::string filename, std::set<std::string> allowedMethods, bool isCgi);
		bool isPathAllowed(const std::string & method, const std::string & path) const;
		bool pathExists(const std::string & path) const;
		bool isCgi(const std::string & path) const;
		size_t count(const std::string & path) const;

	// Operator overloads

		LocationContainer & operator=(const LocationContainer & rhs);
		Location & operator[](const std::string & path);
		Location & operator[](size_t index);

	protected:
	private:
	// Member attributes

		std::vector<Location> _locations;

	// Friends <3

	friend std::ostream &operator<<(std::ostream &os, const LocationContainer &obj);
};

std::ostream &operator<<(std::ostream &os, const LocationContainer &obj);
