#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace CHTL {

class StringUtil {
public:
    // Trim from start (in place)
    static inline void LTrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    // Trim from end (in place)
    static inline void RTrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    // Trim from both ends (in place)
    static inline void Trim(std::string &s) {
        LTrim(s);
        RTrim(s);
    }

    // Split a string by a delimiter
    static inline std::vector<std::string> Split(const std::string &s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
};

} // namespace CHTL

#endif // STRING_UTIL_H
