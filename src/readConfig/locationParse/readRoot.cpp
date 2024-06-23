#include "readConfig.hpp"
#include "utils.hpp"

void readRoot(std::list<std::string>::iterator & it, const std::list<std::string>::iterator & end, Location & loc)
{
	if (it == end)
		throw std::runtime_error("Error reading config file, expected root after root token");
	// merge the root with the existing one bc location inherits root from server
	loc.setRoot(joinPath(loc.getRoot(),*(it++)));
	if (it == end or *it != ";")
		throw std::runtime_error("Error reading config file, expected ; after root");
	++it;
}
