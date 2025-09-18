#include "StringUtil.hpp"
#include <sstream>

namespace CHTL {

std::vector<std::string> StringUtil::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

} // namespace CHTL
