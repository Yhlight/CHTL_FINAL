#ifndef CHTL_STRINGUTIL_H
#define CHTL_STRINGUTIL_H

#include <string>
#include <vector>

namespace CHTL
{
    class StringUtil
    {
    public:
        // Trim from start (in place)
        static void ltrim(std::string &s);

        // Trim from end (in place)
        static void rtrim(std::string &s);

        // Trim from both ends (in place)
        static void trim(std::string &s);

        // Split a string by a delimiter
        static std::vector<std::string> split(const std::string &s, char delimiter);

        // Check if a string starts with a given prefix
        static bool startsWith(const std::string &s, const std::string &prefix);

        // Check if a string ends with a given suffix
        static bool endsWith(const std::string &s, const std::string &suffix);
    };
}

#endif // CHTL_STRINGUTIL_H
