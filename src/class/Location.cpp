#include "Location.hpp"
#include "utils.hpp"

Location::Location(void)
{
	_URI = "";
	_index = "";
	_root = "";
	_isCgi = false;
	_autoIndex = false;
}

Location::Location(const Location & src)
{
	*this = src;
}

Location::~Location()
{}

bool Location::matchesURI(const std::string & path) const
{
	// clean path from any extra slashes or dots, decode it and remove the query string
	std::string cleanpath = cleanPath(decodeURL(path.substr(0,path.find('?'))));
	if (cleanpath.empty())
		return false;

	// add a slash at the beginning if it's not there too match the URI
	if (cleanpath[0] != '/' && _URI[0] == '/')
		cleanpath = '/' + cleanpath;

	// if path contains ".." it is not allowed
	if (cleanpath == ".." ||
		path.find("../") != std::string::npos ||
		path.find("/..") != std::string::npos)
		return false;

	// check as a file
	if (back(_URI) != '/')
	{
		if (cleanpath == _URI || '/' + cleanpath == _URI)
			return true;
		return false;
	}
	
	// check as a directory
	// check if the path starts with the URI
	if (_URI.size() <= cleanpath.size() && cleanpath.compare(0, _URI.size(), _URI) == 0)
		return true;
	// check if the path is the URI with a slash at the end
	if (back(cleanpath)!= '/')
		cleanpath += '/';
	if (cleanpath == _URI)
		return true;

	return false;
}

std::string Location::getFilename(const std::string & path) const
{
	// if the path doesn't match the URI, return an empty string
	if (matchesURI(path) == false)
		return "";

	// clean path from any extra slashes or dots, decode it and remove the query string
	std::string cleanpath = cleanPath(decodeURL(path.substr(0,path.find('?'))));
	if (cleanpath.empty())
		return "";

	// If there is a file alias, return it
	if (_alias.size() > 0 && back(_alias) != '/')
	{
		if (_alias[0] == '/')
			return _alias.substr(1);
		return _alias;
	}
	// If there is a folder alias, return the path with the alias
	if (_alias.size() > 0)
	{
		if (cleanpath[0] == '/')
			cleanpath = cleanpath.substr(1);
		cleanpath = joinPath(_alias, cleanpath.substr(trim(_URI,"/").size()));
		if (cleanpath.size() == 0)
			return "";
		return cleanpath.substr(cleanpath[0] == '/', cleanpath.size() - (back(cleanpath) == '/'));
	}
	// If the location is a file, return the root + path
	if (back(_URI) != '/')
	{
		if (_root.size() == 0)
			return cleanpath;
		if (_root[0] == '/')
			return joinPath(_root, cleanpath).substr(1);
		return joinPath(_root, cleanpath);
	}
	// Add the root to the path
	cleanpath = joinPath(_root, cleanpath);
	if (cleanpath.size() > 0 && cleanpath[0] == '/')
		cleanpath = cleanpath.substr(1);

	// If the path is a directory, add the index file
	if (cleanpath.empty() || isDir(cleanpath) || trim(cleanpath) == trim(joinPath(_root, _URI)))
	{
		if (_index.size() == 0)
			return joinPath(cleanpath, "index.html");
		return joinPath(cleanpath, _index);
	}
	// If the path is a file, return it
	return cleanpath;
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
		_index = rhs._index;
		_root = rhs._root;
		_alias = rhs._alias;
		_allowedMethods = rhs._allowedMethods;
		_isCgi = rhs._isCgi;
		_autoIndex = rhs._autoIndex;
	}
	return (*this);
}

bool Location::operator==(const Location & rhs) const
{
	return (operator==(rhs._URI));
}

bool Location::operator==(const std::string & rhs) const
{
	std::string uri = _URI;
	if (uri.size() > 0 && uri[0] != '/')
		uri = '/' + uri;
	std::string path = rhs;
	if (path.size() > 0 && path[0] != '/')
		path = '/' + path;
	return (path == uri);
}

std::ostream &operator<<(std::ostream &os, const Location &obj)
{
	os << "URI: " << obj._URI;
	os << " | index: " << obj._index;
	os << " | root: " << obj._root;
	os << " | alias: " << obj._alias;
	os << " | allowedMethods: ";
	for (std::set<std::string>::iterator i = obj._allowedMethods.begin(); i != obj._allowedMethods.end(); ++i)
		os << *i << " ";
	os << " | isCgi: " << obj._isCgi;
	os << " | autoIndex: " << obj._autoIndex;
	return (os);
}