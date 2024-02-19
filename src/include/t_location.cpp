#include "t_location.hpp"

void t_location::setPath(const std::string & path)
{
	this->path = path;
}

void t_location::setRoot(const std::string & root)
{
	this->root = root;
}

void t_location::setIndex(const std::string & index)
{
	this->index = index;
}

void t_location::setAutoindex(const std::string & autoindex)
{
	this->autoindex = autoindex;
}

void t_location::setMethods(const std::string & methods)
{
	this->methods = methods;
}

std::string t_location::getPath(void) const
{
	return (path);
}

std::string t_location::getRoot(void) const
{
	return (root);
}

std::string t_location::getIndex(void) const
{
	return (index);
}

std::string t_location::getAutoindex(void) const
{
	return (autoindex);
}

std::string t_location::getMethods(void) const
{
	return (methods);
}