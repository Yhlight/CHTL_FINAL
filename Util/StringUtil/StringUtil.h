#pragma once

#include <string>
#include <vector>

namespace CHTL {

class StringUtil {
public:
    // Splits a string by a delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter);

    // Removes leading and trailing whitespace from a string
    static std::string trim(const std::string& str);

    // Checks if a string starts with a given prefix
    static bool startsWith(const std::string& str, const std::string& prefix);

    // Checks if a string ends with a given suffix
    static bool endsWith(const std::string& str, const std::string& suffix);

    // Replaces all occurrences of a substring with another substring
    static std::string replaceAll(std::string str, const std::string& from, const std::string& to);
};

} // namespace CHTL
