#pragma once

#ifdef DEBUG_MODE
# include <iostream>
# define DEBUG(x) std::cerr << x << std::endl
#else
# define DEBUG(x) void(0)
#endif
