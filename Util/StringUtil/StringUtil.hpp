#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace CHTL {

class StringUtil {
public:
    // Trim from start (in place)
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // Trim from end (in place)
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // Trim from both ends (in place)
    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    // Split a string by a delimiter
    static std::vector<std::string> split(const std::string& s, char delimiter);
};

} // namespace CHTL
