#include "Location.hpp"
#include "utils.hpp"

Location::Location(const std::string & URI)
{
	setURI(URI);
	_isCgi = false;
	_autoIndex = false;
	_returnValue.first = -1;
}

Location::Location(const Location & src)
{
	*this = src;
}

Location::~Location()
{}

void Location::setURI(const std::string & URI)
{
	if (URI.empty() || URI[0] != '/')
		_URI = '/' + URI;
	else
		_URI = URI;
}

const std::string &Location::getURI(void) const
{
	return _URI;
}

void Location::setIndex(const std::string & index)
{
	_index = index;
}

const std::string &Location::getIndex(void) const
{
	return _index;
}

void Location::setRoot(const std::string & root)
{
	_alias.clear();
	if (root.size() > 0 && root[0] != '/')
		_root = '/' + root;
	else
		_root = root;
}

const std::string &Location::getRoot(void) const
{
	return _root;
}

void Location::setAlias(const std::string & alias)
{
	_root.clear();
	if (alias.size() > 0 && alias[0] != '/')
		_alias = '/' + alias;
	else
		_alias = alias;
}

const std::string &Location::getAlias(void) const
{
	return _alias;
}

void Location::setAllowedMethods(const std::set<std::string> & allowedMethods)
{
	_allowedMethods = allowedMethods;
}

void Location::addAllowedMethod(const std::string & method)
{
	_allowedMethods.insert(method);
}

const std::set<std::string> &Location::getAllowedMethods(void) const
{
	return _allowedMethods;
}

bool Location::isAllowedMethod(const std::string & method) const
{
	// if no allowed methods are specified, all methods are allowed
	if (_allowedMethods.empty())
		return true;
	return (_allowedMethods.find(method) != _allowedMethods.end());
}

void Location::isCgi(bool isCgi)
{
	_isCgi = isCgi;
}

bool Location::isCgi(void) const
{
	return _isCgi;
}

void Location::autoIndex(bool autoIndex)
{
	_autoIndex = autoIndex;
}

bool Location::autoIndex(void) const
{
	return _autoIndex;
}

void Location::setReturnValue(int code, const std::string & body)
{
	_returnValue.first = code;
	_returnValue.second = body;
}

const std::pair<int, std::string> &Location::getReturnValue(void) const
{
	return _returnValue;
}

bool Location::hasReturnValue(void) const
{
	return (_returnValue.first != -1);
}

bool Location::isFile() const
{
	return (back(_URI) != '/');
}

#include "colors.h"
bool Location::matchesURI(std::string path) const
{
	if (path.empty())
		return false;

	// add a slash at the beginning if it's not there too match the URI
	if (path[0] != '/')
		path = '/' + path;

	// if path contains ".." it is not allowed
	if (path.find("../") != std::string::npos ||
		path.find("/..") != std::string::npos)
		return false;

	// check as a file
	if (back(_URI) != '/')
	{
		if (path == _URI)
			return true;
		return false;
	}
	
	// check as a directory
	// check if the path starts with the URI
	if (_URI.size() <= path.size() && path.compare(0, _URI.size(), _URI) == 0)
		return true;
	// check if the path is the URI with a slash at the end
	if (back(path)!= '/')
		path += '/';
	if (path == _URI)
		return true;

	return false;
}

std::string Location::getFilename(std::string path) const
{
	if (path.empty())
		return "";

	// If there is a file alias, return it without the first slash
	if (_alias.size() > 0 && back(_alias) != '/')
		return _alias.substr(1);

	// If there is a folder alias, return the path with the alias
	if (_alias.size() > 0)
	{
		if (path[0] == '/')
			path = path.substr(1);
		path = joinPath(_alias, path.substr(trim(_URI,"/").size()));
		if (path.size() == 0)
			return "";
		return path.substr(path[0] == '/', path.size() - (back(path) == '/'));
	}
	// If the location is a file, return the root + path
	if (back(_URI) != '/')
	{
		if (_root.size() == 0)
			return path;
		if (_root[0] == '/')
			return joinPath(_root, path).substr(1);
		return joinPath(_root, path);
	}
	// Add the root to the path
	path = joinPath(_root, path);
	if (path.size() > 0 && path[0] == '/')
		path = path.substr(1);

	// If the path is a directory, add the index file
	if (path.empty() || isDir(path) || trim(path) == trim(joinPath(_root, _URI)))
	{
		if (_index.size() == 0)
			return joinPath(path, "index.html");
		return joinPath(path, _index);
	}
	// If the path is a file, return it
	return path;
}

void Location::clear(void)
{
	_URI.clear();
	_index.clear();
	_root.clear();
	_alias.clear();
	_allowedMethods.clear();
	_isCgi = false;
	_autoIndex = false;
	_returnValue.first = -1;
	_returnValue.second.clear();
}

HttpResponse Location::getAutoIndexResponse(const std::string & path) const
{
	// TODO
	(void)path;
	HttpResponse response;
	response.setStatus(500, "not_yet_implemented");
	return response;
}

HttpResponse Location::getReturnResponse(void) const
{
	HttpResponse response;
	if (_returnValue.first == -1)
		return HttpResponse::error(500,"not_OK","Tried to redirect to a location with no return value");
	response.setStatus(_returnValue.first);
	if (_returnValue.first == 301 || _returnValue.first == 302)
		response.setHeader("Location",_returnValue.second);
	else
		response.setBody("text/plain",_returnValue.second);
	response.setReady(true);
	return response;
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
		_returnValue = rhs._returnValue;
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