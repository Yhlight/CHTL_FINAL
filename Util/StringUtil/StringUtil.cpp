#include "StringUtil.h"
#include <cctype>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <regex>

namespace CHTL {

std::string StringUtil::base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
std::string StringUtil::urlSafeChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

std::string StringUtil::trim(const std::string& str) {
    return trimRight(trimLeft(str));
}

std::string StringUtil::trimLeft(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string StringUtil::trimRight(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
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

std::string StringUtil::capitalize(const std::string& str) {
    if (str.empty()) return str;
    std::string result = toLower(str);
    result[0] = std::toupper(result[0]);
    return result;
}

std::string StringUtil::reverse(const std::string& str) {
    std::string result = str;
    std::reverse(result.begin(), result.end());
    return result;
}

bool StringUtil::contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

bool StringUtil::startsWith(const std::string& str, const std::string& prefix) {
    return str.length() >= prefix.length() && 
           str.substr(0, prefix.length()) == prefix;
}

bool StringUtil::endsWith(const std::string& str, const std::string& suffix) {
    return str.length() >= suffix.length() && 
           str.substr(str.length() - suffix.length()) == suffix;
}

size_t StringUtil::find(const std::string& str, const std::string& substr, size_t pos) {
    return str.find(substr, pos);
}

size_t StringUtil::findLast(const std::string& str, const std::string& substr) {
    return str.rfind(substr);
}

std::string StringUtil::replace(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = result.find(from);
    if (pos != std::string::npos) {
        result.replace(pos, from.length(), to);
    }
    return result;
}

std::string StringUtil::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

std::string StringUtil::replaceRegex(const std::string& str, const std::string& pattern, const std::string& replacement) {
    try {
        std::regex regex_pattern(pattern);
        return std::regex_replace(str, regex_pattern, replacement);
    } catch (const std::exception&) {
        return str;
    }
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

std::vector<std::string> StringUtil::split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    result.push_back(str.substr(start));
    return result;
}

std::vector<std::string> StringUtil::splitLines(const std::string& str) {
    return split(str, '\n');
}

std::string StringUtil::join(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) return "";
    
    std::stringstream ss;
    for (size_t i = 0; i < strings.size() - 1; ++i) {
        ss << strings[i] << delimiter;
    }
    ss << strings.back();
    
    return ss.str();
}

std::string StringUtil::join(const std::vector<std::string>& strings, char delimiter) {
    return join(strings, std::string(1, delimiter));
}

std::string StringUtil::format(const std::string& format, const std::vector<std::string>& args) {
    std::string result = format;
    for (size_t i = 0; i < args.size(); ++i) {
        std::string placeholder = "{" + std::to_string(i) + "}";
        result = replaceAll(result, placeholder, args[i]);
    }
    return result;
}

std::string StringUtil::padLeft(const std::string& str, size_t width, char padding) {
    if (str.length() >= width) return str;
    return std::string(width - str.length(), padding) + str;
}

std::string StringUtil::padRight(const std::string& str, size_t width, char padding) {
    if (str.length() >= width) return str;
    return str + std::string(width - str.length(), padding);
}

std::string StringUtil::padCenter(const std::string& str, size_t width, char padding) {
    if (str.length() >= width) return str;
    
    size_t totalPadding = width - str.length();
    size_t leftPadding = totalPadding / 2;
    size_t rightPadding = totalPadding - leftPadding;
    
    return std::string(leftPadding, padding) + str + std::string(rightPadding, padding);
}

int StringUtil::toInt(const std::string& str, int defaultValue) {
    try {
        return std::stoi(str);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

long StringUtil::toLong(const std::string& str, long defaultValue) {
    try {
        return std::stol(str);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

double StringUtil::toDouble(const std::string& str, double defaultValue) {
    try {
        return std::stod(str);
    } catch (const std::exception&) {
        return defaultValue;
    }
}

bool StringUtil::toBool(const std::string& str, bool defaultValue) {
    std::string lower = toLower(trim(str));
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        return false;
    }
    return defaultValue;
}

std::string StringUtil::toString(int value) {
    return std::to_string(value);
}

std::string StringUtil::toString(long value) {
    return std::to_string(value);
}

std::string StringUtil::toString(double value) {
    return std::to_string(value);
}

std::string StringUtil::toString(bool value) {
    return value ? "true" : "false";
}

bool StringUtil::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        start = 1;
    }
    
    if (start >= str.length()) return false;
    
    bool hasDot = false;
    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

bool StringUtil::isAlpha(const std::string& str) {
    if (str.empty()) return false;
    
    for (char c : str) {
        if (!std::isalpha(c)) {
            return false;
        }
    }
    
    return true;
}

bool StringUtil::isAlphaNumeric(const std::string& str) {
    if (str.empty()) return false;
    
    for (char c : str) {
        if (!std::isalnum(c)) {
            return false;
        }
    }
    
    return true;
}

bool StringUtil::isWhitespace(const std::string& str) {
    if (str.empty()) return false;
    
    for (char c : str) {
        if (!std::isspace(c)) {
            return false;
        }
    }
    
    return true;
}

bool StringUtil::isEmpty(const std::string& str) {
    return str.empty();
}

bool StringUtil::isBlank(const std::string& str) {
    return isWhitespace(str);
}

std::string StringUtil::urlEncode(const std::string& str) {
    std::stringstream ss;
    for (unsigned char c : str) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            ss << c;
        } else {
            ss << '%' << std::hex << std::uppercase << static_cast<int>(c);
        }
    }
    return ss.str();
}

std::string StringUtil::urlDecode(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            std::string hex = str.substr(i + 1, 2);
            char c = static_cast<char>(std::stoi(hex, nullptr, 16));
            result += c;
            i += 2;
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string StringUtil::htmlEncode(const std::string& str) {
    std::string result = str;
    result = replaceAll(result, "&", "&amp;");
    result = replaceAll(result, "<", "&lt;");
    result = replaceAll(result, ">", "&gt;");
    result = replaceAll(result, "\"", "&quot;");
    result = replaceAll(result, "'", "&#39;");
    return result;
}

std::string StringUtil::htmlDecode(const std::string& str) {
    std::string result = str;
    result = replaceAll(result, "&amp;", "&");
    result = replaceAll(result, "&lt;", "<");
    result = replaceAll(result, "&gt;", ">");
    result = replaceAll(result, "&quot;", "\"");
    result = replaceAll(result, "&#39;", "'");
    return result;
}

std::string StringUtil::base64Encode(const std::string& str) {
    std::string result;
    int val = 0, valb = -6;
    for (unsigned char c : str) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(base64Chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        result.push_back(base64Chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (result.size() % 4) {
        result.push_back('=');
    }
    return result;
}

std::string StringUtil::base64Decode(const std::string& str) {
    std::string result;
    int val = 0, valb = -8;
    for (char c : str) {
        if (c == '=') break;
        size_t pos = base64Chars.find(c);
        if (pos == std::string::npos) continue;
        val = (val << 6) + pos;
        valb += 6;
        if (valb >= 0) {
            result.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return result;
}

bool StringUtil::matches(const std::string& str, const std::string& pattern) {
    try {
        std::regex regex_pattern(pattern);
        return std::regex_match(str, regex_pattern);
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> StringUtil::findAll(const std::string& str, const std::string& pattern) {
    std::vector<std::string> result;
    try {
        std::regex regex_pattern(pattern);
        std::sregex_iterator begin(str.begin(), str.end(), regex_pattern);
        std::sregex_iterator end;
        
        for (auto it = begin; it != end; ++it) {
            result.push_back(it->str());
        }
    } catch (const std::exception&) {
        // 忽略正则表达式错误
    }
    return result;
}

std::string StringUtil::extract(const std::string& str, const std::string& pattern) {
    try {
        std::regex regex_pattern(pattern);
        std::smatch match;
        if (std::regex_search(str, match, regex_pattern)) {
            return match.str();
        }
    } catch (const std::exception&) {
        // 忽略正则表达式错误
    }
    return "";
}

std::string StringUtil::substring(const std::string& str, size_t start, size_t length) {
    if (start >= str.length()) return "";
    if (length == std::string::npos || start + length > str.length()) {
        return str.substr(start);
    }
    return str.substr(start, length);
}

std::string StringUtil::insert(const std::string& str, size_t pos, const std::string& substr) {
    if (pos > str.length()) pos = str.length();
    return str.substr(0, pos) + substr + str.substr(pos);
}

std::string StringUtil::remove(const std::string& str, size_t start, size_t length) {
    if (start >= str.length()) return str;
    if (start + length >= str.length()) {
        return str.substr(0, start);
    }
    return str.substr(0, start) + str.substr(start + length);
}

std::string StringUtil::repeat(const std::string& str, int count) {
    if (count <= 0) return "";
    
    std::string result;
    for (int i = 0; i < count; ++i) {
        result += str;
    }
    return result;
}

int StringUtil::compare(const std::string& str1, const std::string& str2, bool caseSensitive) {
    if (caseSensitive) {
        return str1.compare(str2);
    } else {
        return toLower(str1).compare(toLower(str2));
    }
}

bool StringUtil::equals(const std::string& str1, const std::string& str2, bool caseSensitive) {
    return compare(str1, str2, caseSensitive) == 0;
}

bool StringUtil::equalsIgnoreCase(const std::string& str1, const std::string& str2) {
    return equals(str1, str2, false);
}

size_t StringUtil::count(const std::string& str, const std::string& substr) {
    size_t count = 0;
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != std::string::npos) {
        ++count;
        pos += substr.length();
    }
    return count;
}

size_t StringUtil::count(const std::string& str, char c) {
    return std::count(str.begin(), str.end(), c);
}

size_t StringUtil::length(const std::string& str) {
    return str.length();
}

size_t StringUtil::length(const std::wstring& str) {
    return str.length();
}

std::string StringUtil::escape(const std::string& str, const std::string& chars) {
    std::string result = str;
    for (char c : chars) {
        std::string from(1, c);
        std::string to = "\\" + from;
        result = replaceAll(result, from, to);
    }
    return result;
}

std::string StringUtil::unescape(const std::string& str, const std::string& chars) {
    std::string result = str;
    for (char c : chars) {
        std::string from = "\\" + std::string(1, c);
        std::string to(1, c);
        result = replaceAll(result, from, to);
    }
    return result;
}

std::string StringUtil::normalize(const std::string& str) {
    return trim(str);
}

std::string StringUtil::slugify(const std::string& str) {
    std::string result = toLower(str);
    result = replaceRegex(result, "[^a-z0-9\\s-]", "");
    result = replaceRegex(result, "\\s+", "-");
    result = replaceRegex(result, "-+", "-");
    result = trim(result, "-");
    return result;
}

std::string StringUtil::interpolate(const std::string& template_str, const std::map<std::string, std::string>& variables) {
    std::string result = template_str;
    for (const auto& pair : variables) {
        std::string placeholder = "{{" + pair.first + "}}";
        result = replaceAll(result, placeholder, pair.second);
    }
    return result;
}

std::vector<std::string> StringUtil::extractVariables(const std::string& template_str) {
    std::vector<std::string> variables;
    std::regex pattern("\\{\\{([^}]+)\\}\\}");
    std::sregex_iterator begin(template_str.begin(), template_str.end(), pattern);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        variables.push_back((*it)[1].str());
    }
    
    return variables;
}

} // namespace CHTL