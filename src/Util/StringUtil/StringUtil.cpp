#include "Util/StringUtil/StringUtil.h"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace CHTL {

StringUtil::StringUtil() {
}

StringUtil::~StringUtil() = default;

std::string StringUtil::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string StringUtil::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtil::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::vector<std::string> StringUtil::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::vector<std::string> StringUtil::split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    tokens.push_back(str.substr(start));
    return tokens;
}

std::string StringUtil::join(const std::vector<std::string>& tokens, const std::string& delimiter) {
    if (tokens.empty()) {
        return "";
    }
    
    std::string result = tokens[0];
    for (size_t i = 1; i < tokens.size(); ++i) {
        result += delimiter + tokens[i];
    }
    
    return result;
}

std::string StringUtil::replace(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    
    return result;
}

bool StringUtil::startsWith(const std::string& str, const std::string& prefix) {
    return str.length() >= prefix.length() && 
           str.substr(0, prefix.length()) == prefix;
}

bool StringUtil::endsWith(const std::string& str, const std::string& suffix) {
    return str.length() >= suffix.length() && 
           str.substr(str.length() - suffix.length()) == suffix;
}

bool StringUtil::contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

std::string StringUtil::escape(const std::string& str) {
    std::string result = str;
    result = replace(result, "\\", "\\\\");
    result = replace(result, "\"", "\\\"");
    result = replace(result, "\n", "\\n");
    result = replace(result, "\r", "\\r");
    result = replace(result, "\t", "\\t");
    return result;
}

std::string StringUtil::unescape(const std::string& str) {
    std::string result = str;
    result = replace(result, "\\t", "\t");
    result = replace(result, "\\r", "\r");
    result = replace(result, "\\n", "\n");
    result = replace(result, "\\\"", "\"");
    result = replace(result, "\\\\", "\\");
    return result;
}

} // namespace CHTL