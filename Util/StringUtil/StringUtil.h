#ifndef CHTL_STRING_UTIL_H
#define CHTL_STRING_UTIL_H

#include <string>
#include <string_view>

// Trims leading and trailing whitespace from a string_view and returns a new string.
std::string trim(std::string_view s);

#endif // CHTL_STRING_UTIL_H
