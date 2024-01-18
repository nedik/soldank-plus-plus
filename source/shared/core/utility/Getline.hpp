#ifndef __GETLINE_HPP__
#define __GETLINE_HPP__

#include <string>
#include <fstream>
#include <cstdlib>

namespace Soldat
{
std::istream& GetlineSafe(std::istream& is, std::string& t);
} // namespace Soldat

#endif
