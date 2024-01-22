#include "Locations.hpp"
#include "utils.hpp"
#include "colors.h"

bool Locations::Location::isPathAllowed(const std::string & path) const
{
	// if path contains "../" it is not allowed
	if (path.find("../") != std::string::npos)
		return false;

	// if path is a folder is not allowed
	size_t len = path.size();
	if (len > 1 && path[len - 1] == '/')
		return false;

	// check as a file
	if (_isFile)
	{
		if (path == _path)
			return true;
		return false;
	}
	
	// check as a directory
	// create a copy of this->_path with a slash at the end if it doesn't have one
	std::string path_with_slash = _path;
	if (back(_path) != '/')
		path_with_slash += '/';
	
	// check that path starts with this->_path and has something after it
	if (path.compare(0, path_with_slash.size(), path_with_slash) == 0	// if path starts with allowed path
		&& path.size() > path_with_slash.size())	// if path is longer than allowed path (is a file in the directory)
			return true;
	// if nothing matches return false
	return false;
}

bool Locations::Location::isMethodAllowed(const std::string & method) const
{
	// if no allowed methods are specified, all methods are allowed
	if (_allowedMethods.empty())
		return true;
	return (_allowedMethods.find(method) != _allowedMethods.end());
}

bool Locations::Location::operator==(const Location & rhs) const
{
	return (_path == rhs._path);
}

Locations::Locations(void)
{}

Locations::Locations(const Locations & src)
{
	*this = src;
}

Locations::~Locations()
{}

static std::string construct_path(const std::string & path)
{
	size_t len = path.size();
	if (len >= 2 && path[0] == '.' && path[1] == '/')
		return path;
	if (len >= 1 && path[0] == '/')
		return std::string (".") + path;
	return std::string ("./") + path;
}

std::string Locations::getFilename(const std::string & method, const std::string & path) const
{
	size_t len = _locations.size();
	for (size_t i = 0; i < len; ++i)
	{
		if (_locations[i].isPathAllowed(path) && _locations[i].isMethodAllowed(method))
		{
			if (_locations[i]._filename == "")
				return construct_path(path);
			return construct_path(_locations[i]._filename);
		}
	}
	return "";
}

bool Locations::addLocation(const Locations::Location & location)
{
	// check if location already exists
	size_t len = _locations.size();
	for (size_t i = 0; i < len; ++i)
		if (_locations[i] == location)
			return false;
	
	// add location
	_locations.push_back(location);
	return true;
}

bool Locations::addLocation(std::string path, bool isFile, std::string filename, std::set<std::string> allowedMethods)
{
	// check if location already exists
	size_t len = _locations.size();
	for (size_t i = 0; i < len; ++i)
		if (_locations[i]._path == path)
			return false;
	
	// add location
	Location location;
	location._path = path;
	location._isFile = isFile;
	location._filename = filename;
	location._allowedMethods = allowedMethods;
	_locations.push_back(location);
	return true;
}

bool Locations::isPathAllowed(const std::string & method, const std::string & path) const {
	size_t len = _locations.size();
	for (size_t i = 0; i < len; ++i)
		if (_locations[i].isPathAllowed(path) && _locations[i].isMethodAllowed(method))
			return true;
	return false;
}

Locations &Locations::operator=(const Locations &rhs)
{
	if (this != &rhs)
	{
		_locations = rhs._locations;
	}
	return (*this);
}

Locations::Location &Locations::operator[](size_t index)
{
	return (_locations[index]);
}

std::ostream &operator<<(std::ostream &os, const Locations &obj)
{
	size_t len = obj._locations.size();
	for (size_t i = 0; i < len; ++i)
	{
		os << "[" << i << "]:\n";
		os << "\tpath: " << obj._locations[i]._path << "\n";
		if (obj._locations[i]._isFile)
			os << "\tfile\n";
		else
			os << "\tdirectory\n";
		os << "\tfilename: " << obj._locations[i]._filename << "\n";
		os << "\tallowedMethods: ";
		std::set<std::string>::iterator it;
		for (it = obj._locations[i]._allowedMethods.begin();
			it != obj._locations[i]._allowedMethods.end();)
		{
			os << *it;
			++it;
			if (it != obj._locations[i]._allowedMethods.end())
				os << ", ";
		}
		os << std::endl;
	}
	return (os);
}
