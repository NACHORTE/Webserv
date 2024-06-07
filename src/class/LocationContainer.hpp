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

		// Get the location that best matches the path (throws if no match is found) O(n)
		const Location * getLocation(const std::string & path) const;
	// Member functions

		// returns the filename that best matches the path. Returns empty string if no match is found
		std::string getFilename(const std::string & path) const; 
		bool addLocation(const Location & location);
		bool matchesURI(const std::string & method, const std::string & path) const; // NOTE MAYBE NOT NEEDED, getFilename checks too

	// Operator overloads

		LocationContainer & operator=(const LocationContainer & rhs);
		// Get the location that best matches the path (throws if no match is found) O(n)
		const Location * operator[](const std::string & path) const;
		// Get the location at index (throws if index is out of bounds) O(1)
		const Location * operator[](size_t index) const;

	protected:
	private:
	// Member attributes

		std::vector<Location> _locations;

	// Friends <3

	friend std::ostream &operator<<(std::ostream &os, const LocationContainer &obj);
};

std::ostream &operator<<(std::ostream &os, const LocationContainer &obj);
