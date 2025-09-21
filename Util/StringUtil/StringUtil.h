#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <vector>
#include <string>

namespace CHTL {

class StringUtil {
public:
    static std::vector<std::string> split(const std::string& s, char delimiter);
    static std::string getFilename(const std::string& path);
};

} // namespace CHTL

#endif // STRING_UTIL_H
