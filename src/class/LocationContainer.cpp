#include "LocationContainer.hpp"
#include "utils.hpp"
#include "colors.h"

LocationContainer::LocationContainer(void)
{}

LocationContainer::LocationContainer(const LocationContainer & src)
{
	*this = src;
}

LocationContainer::~LocationContainer()
{}

static std::string formatPath(const std::string & path)
{
	size_t len = path.size();
	if (len >= 2 && path[0] == '.' && path[1] == '/')
		return path;
	if (len >= 1 && path[0] == '/')
		return std::string (".") + path;
	return std::string ("./") + path;
}

std::string LocationContainer::getFilename(const std::string & path) const
{
	for (size_t i = 0; i < _locations.size();++i)
		if (_locations[i].isPathAllowed(path))
		{
			if (_locations[i]._filename == "")
				return formatPath(path);
			return formatPath(_locations[i]._filename);
		}
	return "";
}

bool LocationContainer::addLocation(const Location & location)
{
	// check if location already exists
	for (size_t i = 0; i < _locations.size(); ++i)
		if (_locations[i] == location)
			return false;

	// add location
	_locations.push_back(location);
	return true;
}

bool LocationContainer::addLocation(std::string path, bool isFile, std::string filename, std::set<std::string> allowedMethods, bool isCgi)
{
	// check if location already exists
	for (size_t i = 0; i < _locations.size(); ++i)
		if (_locations[i] == path)
			return false;

	// add location
	Location location;
	location._URI = path;
	location._isFile = isFile;
	location._filename = filename;
	location._allowedMethods = allowedMethods;
	location._isCgi = isCgi;
	//location._autoIndex = false; //TODO
	_locations.push_back(location);
	return true;
}

bool LocationContainer::isPathAllowed(const std::string & method, const std::string & path) const
{
	for (std::vector<Location>::const_iterator i = _locations.begin(); i != _locations.end(); ++i)
		if (i->isPathAllowed(path) && i->isMethodAllowed(method))
			return true;
	return false;
}

bool LocationContainer::pathExists(const std::string & path) const
{
	std::vector<Location>::const_iterator i;
	for (i = _locations.begin(); i != _locations.end(); ++i)
	{
		// If is file and path perfectly matches, return true
		if (i->_isFile && i->_URI == path)
			return true;
		// If is folder and path starts with i->_path, return true
		if (!i->_isFile// if is folder
			&& path.compare(0, i->_URI.size(), i->_URI) == 0	// if path starts with allowed path
			&& path.size() > i->_URI.size())	// if path is longer than allowed path (is a file in the directory)
			return true;
	}
	return false;
}

bool LocationContainer::isCgi(const std::string & path) const
{
	std::vector<Location>::const_iterator i;
	for (i = _locations.begin(); i != _locations.end(); ++i)
	{
		if (i->isPathAllowed(path) && i->_isCgi)
			return true;
	}
	return false;
}

LocationContainer &LocationContainer::operator=(const LocationContainer &rhs)
{
	if (this != &rhs)
	{
		_locations = rhs._locations;
	}
	return (*this);
}

Location &LocationContainer::operator[](const std::string & path)
{
	for (size_t i = 0; i < _locations.size(); ++i)
		if (_locations[i] == path)
			return _locations[i];
	throw std::out_of_range("Location not found for path: " + path);
}

Location & LocationContainer::operator[](size_t index)
{
	return _locations[index];
}

std::ostream &operator<<(std::ostream &os, const LocationContainer &obj)
{
	size_t i = 0;
	for (std::vector<Location>::const_iterator it = obj._locations.begin(); it !=obj._locations.end(); ++it)
	{
		os << i++ << ". "<< BOLD << *it << RESET << "\n";
	}
	return (os);
}
