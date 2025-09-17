#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <locale>

namespace StringUtil {
    // Trim from start (in place)
    inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // Trim from end (in place)
    inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // Trim from both ends (in place)
    inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    // Trim from start (copy)
    inline std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    // Trim from end (copy)
    inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    // Trim from both ends (copy)
    inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }

    // Split a string by a delimiter
    std::vector<std::string> split(const std::string &s, char delimiter);

    // Check if a string starts with a prefix
    inline bool startsWith(const std::string& str, const std::string& prefix) {
        return str.rfind(prefix, 0) == 0;
    }

    // Check if a string ends with a suffix
    inline bool endsWith(const std::string& str, const std::string& suffix) {
        return suffix.length() <= str.length() && str.substr(str.length() - suffix.length()) == suffix;
    }

    // Convert string to lower case
    inline std::string toLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return s;
    }

    // Replace all occurrences of a substring
    void replaceAll(std::string& str, const std::string& from, const std::string& to);
}

#endif // STRING_UTIL_H
