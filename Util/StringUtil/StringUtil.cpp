#include "StringUtil.h"
#include <algorithm>
#include <cctype>
#include <locale>
#include <sstream>

namespace CHTL
{
    void StringUtil::ltrim(std::string &s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
    }

    void StringUtil::rtrim(std::string &s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    void StringUtil::trim(std::string &s)
    {
        ltrim(s);
        rtrim(s);
    }

    std::vector<std::string> StringUtil::split(const std::string &s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    bool StringUtil::startsWith(const std::string &s, const std::string &prefix)
    {
        return s.rfind(prefix, 0) == 0;
    }

    bool StringUtil::endsWith(const std::string &s, const std::string &suffix)
    {
        if (suffix.length() > s.length())
        {
            return false;
        }
        return s.rfind(suffix) == (s.length() - suffix.length());
    }
}
