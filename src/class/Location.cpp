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
