#include "Location.hpp"

Location::Location(void)
{
}

Location::Location(const Location & src)
{
	*this = src;
}

Location::~Location()
{
}

Location &Location::operator=(const Location &rhs)
{
	if (this != &rhs)
	{
		// copy
	}
	return (*this);
}

std::ostream &operator<<(std::ostream &os, const Location &obj)
{
	(void)obj;
	return (os);
}

void Location::setPath(const std::string &path)
{
	_uri = path;
}

void Location::setRoot(const std::string &root)
{
	_root = root;
}

void Location::setIndex(const std::string &index)
{
	_index = index;
}

void Location::setAutoindex(const std::string &autoindex)
{
	_autoindex = autoindex;
}

void Location::setAllowMethods(const std::vector<std::string> &allowMethods)
{
	_allowMethods = allowMethods;
}

void Location::addAllowMethod(const std::string &allowMethod)
{
	_allowMethods.push_back(allowMethod);
}

const std::string &Location::getPath() const
{
	return (_uri);
}

const std::string &Location::getRoot() const
{
	return (_root);
}

const std::string &Location::getIndex() const
{
	return (_index);
}

const std::string &Location::getAutoindex() const
{
	return (_autoindex);
}

const std::vector<std::string> &Location::getAllowMethods() const
{
	return (_allowMethods);
}

