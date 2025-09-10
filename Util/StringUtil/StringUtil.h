#ifndef CHTL_STRINGUTIL_H
#define CHTL_STRINGUTIL_H

#include <string>
#include <vector>

namespace StringUtil {

    // Trims whitespace from both ends of a string
    std::string trim(const std::string& str);

    // Splits a string by a delimiter
    std::vector<std::string> split(const std::string& str, char delimiter);

    // Checks if a string starts with a given prefix
    bool startsWith(const std::string& str, const std::string& prefix);

    // Checks if a string ends with a given suffix
    bool endsWith(const std::string& str, const std::string& suffix);

} // namespace StringUtil

#endif //CHTL_STRINGUTIL_H
