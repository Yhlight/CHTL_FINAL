#include "StringUtil.h"
#include <algorithm>
#include <cctype>

namespace CHTL {
namespace Util {

// Trim from start (in place)
void StringUtil::ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// Trim from end (in place)
void StringUtil::rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// Trim from both ends (in place)
void StringUtil::trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

} // namespace Util
} // namespace CHTL
