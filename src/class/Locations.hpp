#pragma once
#include <iostream>
#include <vector>
#include <set>
#include <string>

class Locations
{
	public:
		// struct with location data
		class Location;

		// Constructors and destructor
		Locations(void);
		Locations(const Locations & src);
		~Locations();

		// Setters and getters
		std::string getFilename(const std::string & path) const; // returns the filename associated with the path. Returns empty string if no match is found

		// Member functions
		bool addLocation(const Locations::Location & location);
		bool addLocation(std::string path, bool isFile, std::string filename, std::set<std::string> allowedMethods, bool isCgi);
		bool isPathAllowed(const std::string & method, const std::string & path) const;
		bool pathExists(const std::string & path) const;
		bool isCgi(const std::string & path) const;
		// Operator overloads
		Locations & operator=(const Locations & rhs);
		Locations::Location & operator[](size_t index);
	protected:
	private:
		std::vector<Location> _locations;

	friend std::ostream &operator<<(std::ostream &os, const Locations &obj);
};

std::ostream &operator<<(std::ostream &os, const Locations &obj);


class Locations::Location
{
	public:
		bool isPathAllowed(const std::string & path) const;
		bool isMethodAllowed(const std::string & method) const;
		bool operator==(const Location & rhs) const;
		std::string _path;
		bool _isFile;
		std::string _filename;
		std::set<std::string> _allowedMethods;
		bool _isCgi;
	protected:
	private:
};