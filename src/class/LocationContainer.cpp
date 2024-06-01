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

const Location &LocationContainer::getLocation(const std::string & path) const
{
	return operator[](path);
}

std::string LocationContainer::getFilename(const std::string & path) const
{
	try
	{
		const Location & loc = operator[](path);
		return loc.getFilename(path);
	}
	catch (std::runtime_error & e)
	{
		return "";
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

bool LocationContainer::matchesURI(const std::string & method, const std::string & path) const
{
	for (std::vector<Location>::const_iterator i = _locations.begin(); i != _locations.end(); ++i)
		if (i->matchesURI(path) && i->isAllowedMethod(method))
			return true;
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

const Location &LocationContainer::operator[](const std::string & path) const
{
	const Location *bestMatch = NULL;

	for (size_t i = 0; i < _locations.size();++i)
	{
		if (_locations[i].matchesURI(path))
		{
			if (!bestMatch)
				bestMatch = &_locations[i];
			else if (_locations[i].getURI().size() > bestMatch->getURI().size()
				|| _locations[i].isFile())
				bestMatch = &_locations[i];
		}
	}
	if (!bestMatch)
		throw std::runtime_error("No location found for path: " + path);
	return *bestMatch;
}

const Location & LocationContainer::operator[](size_t index) const
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
