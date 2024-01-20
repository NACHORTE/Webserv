#include "Location.hpp"
#include "utils.hpp"

Location::Location(void)
{}

Location::Location(const std::string &path)
{
	this->_path = path;
	if (path == "/")
	{
		this->_isFile = false;
		this->_Filename = "/html/index.html";
	}
	else if (back(path) == '/')
	{
		this->_isFile = false;
		this->_Filename = "";
	}
	else
		this->_isFile = true;
}

Location::Location(const std::string &path,
	bool isFile,
	const std::string &filename,
	const std::vector<std::string> &allowedMethods)
{
	this->_path = path;
	this->_isFile = isFile;
	this->_Filename = filename;
	this->_allowedMethods = allowedMethods;
}

Location::Location(const Location & src)
{
	*this = src;
}

Location::~Location()
{}

void Location::setPath(const std::string & path)
{
	this->_path = path;
}

void Location::setIsFile(bool isFile)
{
	this->_isFile = isFile;
}

void Location::setFilename(const std::string & filename)
{
	this->_Filename = filename;
}

void Location::setAllowedMethods(const std::vector<std::string> & allowedMethods)
{
	this->_allowedMethods = allowedMethods;
}

void Location::addAllowedMethod(const std::string & method)
{
	this->_allowedMethods.push_back(method);
}

const std::string & Location::getPath() const
{
	return (this->_path);
}

const std::string & Location::getFilename() const
{
	return (this->_Filename);
}

const std::vector<std::string> & Location::getAllowedMethods() const
{
	return (this->_allowedMethods);
}

bool Location::isFile() const
{
	return (this->_isFile);
}

bool Location::isMethodAllowed(const std::string & method) const
{
	size_t len = _allowedMethods.size();
	for (size_t i = 0; i < len; i++)
	{
		if (_allowedMethods[i] == method)
			return (true);
	}
	return (false);
}

bool Location::isPathAllowed(const std::string & path) const {
	//compare as file
	if (_isFile)
	{
		if (path == _path)
			return true;
		return false;
	}

	//compare as directory
	// if path contains ".." it is not allowed
	if (path.find("..") != std::string::npos)
		return false;

	// create a copy of this->_path with a slash at the end if it doesn't have one
	std::string path_with_slash = _path;
	if (back(_path) != '/')
		path_with_slash += '/';

	// check that path starts with this->_path and has something after it
	if (path.compare(0, _path.size(), _path) == 0	// if path starts with allowed path
		&& path.size() > _path.size())	// if path is longer than allowed path (is a file in the directory)
			return true;

	// if nothing matches return false
	return false;
}

Location &Location::operator=(const Location &rhs)
{
	if (this != &rhs)
	{
		// copy
	}
	return (*this);
}

bool Location::operator==(const Location &rhs) const
{
	return (rhs._path == this->_path);
}

std::ostream &operator<<(std::ostream &os, const Location &obj)
{
	(void)obj;
	return (os);
}
