#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>

namespace CHTL {

class StringUtil {
public:
    // Splits a string by a delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter);
    // Trims whitespace from both ends of a string
    static std::string trim(const std::string& str);
};

} // namespace CHTL

#endif // STRING_UTIL_H
