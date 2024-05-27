#include "Location.hpp"
#include "utils.hpp"

Location::Location(void)
{}

Location::Location(const Location & src)
{
	*this = src;
}

Location::Location(const std::string & URI)
{
	_URI = URI;
	_filename = "";
	_index = "";
	_root = "";
	_isFile = false;
	_isCgi = false;
	_autoIndex = false;
}

Location::~Location()
{}

bool Location::isPathAllowed(const std::string & path) const
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
		if (path == _URI)
			return true;
		return false;
	}
	
	// check as a directory
	// create a copy of this->_path with a slash at the end if it doesn't have one
	std::string path_with_slash = _URI;
	if (back(_URI) != '/')
		path_with_slash += '/';
	
	// check that path starts with this->_path and has something after it
	if (path.compare(0, path_with_slash.size(), path_with_slash) == 0	// if path starts with allowed path
		&& path.size() > path_with_slash.size())	// if path is longer than allowed path (is a file in the directory)
			return true;
	// if nothing matches return false
	return false;
}

bool Location::isMethodAllowed(const std::string & method) const
{
	// if no allowed methods are specified, all methods are allowed
	if (_allowedMethods.empty())
		return true;
	return (_allowedMethods.find(method) != _allowedMethods.end());
}

Location &Location::operator=(const Location &rhs)
{
	if (this != &rhs)
	{
		_URI = rhs._URI;
		_filename = rhs._filename;
		_index = rhs._index;
		_root = rhs._root;
		_isFile = rhs._isFile;
		_allowedMethods = rhs._allowedMethods;
		_isCgi = rhs._isCgi;
		_autoIndex = rhs._autoIndex;
	}
	return (*this);
}

bool Location::operator==(const Location & rhs) const
{
	return (_URI == rhs._URI);
}

bool Location::operator==(const std::string & rhs) const
{
	return (_URI == rhs);
}

bool Location::operator<(const Location & rhs) const
{
	return (_URI < rhs._URI);
}

std::ostream &operator<<(std::ostream &os, const Location &obj)
{
	os << "URI: " << obj._URI;
	os << " | filename: " << obj._filename;
	os << " | index: " << obj._index;
	os << " | root: " << obj._root;
	os << " | isFile: " << obj._isFile;
	os << " | allowedMethods: ";
	for (std::set<std::string>::iterator i = obj._allowedMethods.begin(); i != obj._allowedMethods.end(); ++i)
		os << *i << " ";
	os << " | isCgi: " << obj._isCgi;
	os << " | autoIndex: " << obj._autoIndex;
	return (os);
}