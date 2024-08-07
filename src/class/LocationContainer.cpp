#include "LocationContainer.hpp"
#include "utils.hpp"

LocationContainer::LocationContainer(void)
{}

LocationContainer::LocationContainer(const LocationContainer & src)
{
	*this = src;
}

LocationContainer::~LocationContainer()
{}

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

size_t LocationContainer::size(void) const
{
	return _locations.size();
}

LocationContainer &LocationContainer::operator=(const LocationContainer &rhs)
{
	if (this != &rhs)
	{
		_locations = rhs._locations;
	}
	return (*this);
}

const Location * LocationContainer::operator[](const std::string & path) const
{
	const Location *bestMatch = NULL;

	for (size_t i = 0; i < _locations.size();++i)
	{
		if (_locations[i].matchesURI(path))
		{
			if (!bestMatch)
				bestMatch = &_locations[i];
			// if perfect match bc location is for file
			else if (back(_locations[i].getURI()) != '/')
			{
				bestMatch = &_locations[i];
				break;
			}
			// If new match has URI and extension longer than best match replace it
			if (_locations[i].getURI().size() >= bestMatch->getURI().size()
				and _locations[i].getExtension().size() >= bestMatch->getExtension().size())
				bestMatch = &_locations[i];
		}
	}
	return bestMatch;
}

const Location * LocationContainer::operator[](size_t index) const
{
	if (index >= _locations.size())
		return NULL;
	return &_locations[index];
}

std::ostream &operator<<(std::ostream &os, const LocationContainer &obj)
{
	size_t i = 0;
	for (std::vector<Location>::const_iterator it = obj._locations.begin(); it !=obj._locations.end(); ++it)
		os << i++ << ". "<< *it << "\n";
	return (os);
}
