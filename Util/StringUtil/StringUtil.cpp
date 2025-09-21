#include "StringUtil.h"
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

std::string StringUtil::getFilename(const std::string& path) {
    size_t last_slash_idx = path.find_last_of("/\\");
    if (std::string::npos != last_slash_idx) {
        std::string filename = path.substr(last_slash_idx + 1);
        size_t dot_idx = filename.find_last_of('.');
        if (std::string::npos != dot_idx) {
            return filename.substr(0, dot_idx);
        }
        return filename;
    }
    return "";
}

} // namespace CHTL
