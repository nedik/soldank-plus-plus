#ifndef __GETLINE_HPP__
#define __GETLINE_HPP__

#include <string>
#include <fstream>
#include <cstdlib>

namespace Soldank
{
std::istream& GetlineSafe(std::istream& is, std::string& t);
} // namespace Soldank

#endif
