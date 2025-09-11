#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>
#include <vector>

class StringUtil {
public:
    static std::string trim(const std::string& str);
    static std::string toLowerCase(const std::string& str);
    static std::string toUpperCase(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string join(const std::vector<std::string>& vec, const std::string& delimiter);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static bool contains(const std::string& str, const std::string& substr);
    static std::string escape(const std::string& str);
    static std::string unescape(const std::string& str);
};

#endif // STRINGUTIL_H