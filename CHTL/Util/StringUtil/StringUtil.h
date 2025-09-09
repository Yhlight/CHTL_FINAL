#pragma once

#include <string>
#include <vector>

namespace CHTL {

class StringUtil {
public:
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string trim(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
};

} // namespace CHTL
