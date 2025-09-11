#include "StringUtil.h"
#include <algorithm>
#include <sstream>

std::string StringUtil::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::string StringUtil::toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtil::toUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string StringUtil::toLower(const std::string& str) {
    return toLowerCase(str);
}

std::string StringUtil::toUpper(const std::string& str) {
    return toUpperCase(str);
}

std::vector<std::string> StringUtil::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    return result;
}

std::string StringUtil::join(const std::vector<std::string>& vec, const std::string& delimiter) {
    if (vec.empty()) return "";
    std::string result = vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        result += delimiter + vec[i];
    }
    return result;
}

bool StringUtil::startsWith(const std::string& str, const std::string& prefix) {
    return str.length() >= prefix.length() && str.substr(0, prefix.length()) == prefix;
}

bool StringUtil::endsWith(const std::string& str, const std::string& suffix) {
    return str.length() >= suffix.length() && str.substr(str.length() - suffix.length()) == suffix;
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

bool StringUtil::contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

std::string StringUtil::escape(const std::string& str) {
    std::string result = str;
    result = replace(result, "\\", "\\\\");
    result = replace(result, "\"", "\\\"");
    result = replace(result, "\n", "\\n");
    result = replace(result, "\t", "\\t");
    return result;
}

std::string StringUtil::unescape(const std::string& str) {
    std::string result = str;
    result = replace(result, "\\n", "\n");
    result = replace(result, "\\t", "\t");
    result = replace(result, "\\\"", "\"");
    result = replace(result, "\\\\", "\\");
    return result;
}