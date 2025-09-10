#ifndef CHTL_STRINGUTIL_H
#define CHTL_STRINGUTIL_H

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

namespace StringUtil {
    // Splits a string by a delimiter.
    std::vector<std::string> split(const std::string& s, char delimiter);

    // Trims from the start of the string (in place).
    void ltrim(std::string &s);

    // Trims from the end of the string (in place).
    void rtrim(std::string &s);

    // Trims from both ends of the string (in place).
    void trim(std::string &s);

} // namespace StringUtil

#endif // CHTL_STRINGUTIL_H
