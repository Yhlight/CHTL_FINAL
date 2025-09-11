#pragma once

#include <string>
#include <vector>

namespace CHTL {

class StringUtil {
public:
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string trim(const std::string& str);
};

} // namespace CHTL
