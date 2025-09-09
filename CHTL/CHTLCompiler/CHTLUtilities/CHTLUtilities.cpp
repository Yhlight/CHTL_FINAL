#include "CHTLUtilities.h"

#include <filesystem>
#include <algorithm>
#include <regex>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

namespace CHTL {

// FileSystem implementation
bool FileSystem::exists(const std::string& path) {
    try {
        return std::filesystem::exists(path);
    } catch (...) {
        return false;
    }
}

bool FileSystem::isFile(const std::string& path) {
    try {
        return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
    } catch (...) {
        return false;
    }
}

bool FileSystem::isDirectory(const std::string& path) {
    try {
        return std::filesystem::exists(path) && std::filesystem::is_directory(path);
    } catch (...) {
        return false;
    }
}

bool FileSystem::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directory(path);
    } catch (...) {
        return false;
    }
}

bool FileSystem::createDirectories(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

bool FileSystem::remove(const std::string& path) {
    try {
        return std::filesystem::remove(path);
    } catch (...) {
        return false;
    }
}

bool FileSystem::removeAll(const std::string& path) {
    try {
        return std::filesystem::remove_all(path) > 0;
    } catch (...) {
        return false;
    }
}

bool FileSystem::copy(const std::string& source, const std::string& destination) {
    try {
        std::filesystem::copy(source, destination);
        return true;
    } catch (...) {
        return false;
    }
}

bool FileSystem::move(const std::string& source, const std::string& destination) {
    try {
        std::filesystem::rename(source, destination);
        return true;
    } catch (...) {
        return false;
    }
}

size_t FileSystem::getFileSize(const std::string& path) {
    try {
        if (isFile(path)) {
            return std::filesystem::file_size(path);
        }
        return 0;
    } catch (...) {
        return 0;
    }
}

std::string FileSystem::getFileName(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

std::string FileSystem::getDirectory(const std::string& path) {
    return std::filesystem::path(path).parent_path().string();
}

std::string FileSystem::getExtension(const std::string& path) {
    return std::filesystem::path(path).extension().string();
}

std::string FileSystem::getAbsolutePath(const std::string& path) {
    try {
        return std::filesystem::absolute(path).string();
    } catch (...) {
        return path;
    }
}

std::string FileSystem::getRelativePath(const std::string& path, const std::string& base) {
    try {
        return std::filesystem::relative(path, base).string();
    } catch (...) {
        return path;
    }
}

std::vector<std::string> FileSystem::listFiles(const std::string& directory) {
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } catch (...) {
        // Ignore errors
    }
    return files;
}

std::vector<std::string> FileSystem::listDirectories(const std::string& directory) {
    std::vector<std::string> directories;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_directory()) {
                directories.push_back(entry.path().string());
            }
        }
    } catch (...) {
        // Ignore errors
    }
    return directories;
}

std::vector<std::string> FileSystem::findFiles(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> files;
    try {
        std::regex regex_pattern(pattern);
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (std::regex_match(filename, regex_pattern)) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (...) {
        // Ignore errors
    }
    return files;
}

std::string FileSystem::readFile(const std::string& path) {
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    } catch (...) {
        return "";
    }
}

bool FileSystem::writeFile(const std::string& path, const std::string& content) {
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        return true;
    } catch (...) {
        return false;
    }
}

bool FileSystem::appendFile(const std::string& path, const std::string& content) {
    try {
        std::ofstream file(path, std::ios::app);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        return true;
    } catch (...) {
        return false;
    }
}

// StringUtil implementation
std::string StringUtil::trim(const std::string& str) {
    return ltrim(rtrim(str));
}

std::string StringUtil::ltrim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string StringUtil::rtrim(const std::string& str) {
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

std::string StringUtil::join(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) {
        return "";
    }
    
    std::string result = strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
        result += delimiter + strings[i];
    }
    return result;
}

bool StringUtil::startsWith(const std::string& str, const std::string& prefix) {
    return str.length() >= prefix.length() && str.substr(0, prefix.length()) == prefix;
}

bool StringUtil::endsWith(const std::string& str, const std::string& suffix) {
    return str.length() >= suffix.length() && str.substr(str.length() - suffix.length()) == suffix;
}

bool StringUtil::contains(const std::string& str, const std::string& substring) {
    return str.find(substring) != std::string::npos;
}

std::string StringUtil::substring(const std::string& str, size_t start, size_t length) {
    if (start >= str.length()) {
        return "";
    }
    if (length == std::string::npos || start + length > str.length()) {
        return str.substr(start);
    }
    return str.substr(start, length);
}

std::string StringUtil::padLeft(const std::string& str, size_t width, char pad) {
    if (str.length() >= width) {
        return str;
    }
    return std::string(width - str.length(), pad) + str;
}

std::string StringUtil::padRight(const std::string& str, size_t width, char pad) {
    if (str.length() >= width) {
        return str;
    }
    return str + std::string(width - str.length(), pad);
}

std::string StringUtil::padCenter(const std::string& str, size_t width, char pad) {
    if (str.length() >= width) {
        return str;
    }
    size_t total_padding = width - str.length();
    size_t left_padding = total_padding / 2;
    size_t right_padding = total_padding - left_padding;
    return std::string(left_padding, pad) + str + std::string(right_padding, pad);
}

std::string StringUtil::escape(const std::string& str) {
    std::string result = str;
    result = replaceAll(result, "\\", "\\\\");
    result = replaceAll(result, "\"", "\\\"");
    result = replaceAll(result, "\n", "\\n");
    result = replaceAll(result, "\r", "\\r");
    result = replaceAll(result, "\t", "\\t");
    return result;
}

std::string StringUtil::unescape(const std::string& str) {
    std::string result = str;
    result = replaceAll(result, "\\t", "\t");
    result = replaceAll(result, "\\r", "\r");
    result = replaceAll(result, "\\n", "\n");
    result = replaceAll(result, "\\\"", "\"");
    result = replaceAll(result, "\\\\", "\\");
    return result;
}

std::string StringUtil::escapeHtml(const std::string& str) {
    std::string result = str;
    result = replaceAll(result, "&", "&amp;");
    result = replaceAll(result, "<", "&lt;");
    result = replaceAll(result, ">", "&gt;");
    result = replaceAll(result, "\"", "&quot;");
    result = replaceAll(result, "'", "&#39;");
    return result;
}

std::string StringUtil::unescapeHtml(const std::string& str) {
    std::string result = str;
    result = replaceAll(result, "&#39;", "'");
    result = replaceAll(result, "&quot;", "\"");
    result = replaceAll(result, "&gt;", ">");
    result = replaceAll(result, "&lt;", "<");
    result = replaceAll(result, "&amp;", "&");
    return result;
}

bool StringUtil::isNumeric(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    for (char c : str) {
        if (!std::isdigit(c) && c != '.' && c != '-' && c != '+') {
            return false;
        }
    }
    return true;
}

bool StringUtil::isAlpha(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    for (char c : str) {
        if (!std::isalpha(c)) {
            return false;
        }
    }
    return true;
}

bool StringUtil::isAlphaNumeric(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    for (char c : str) {
        if (!std::isalnum(c)) {
            return false;
        }
    }
    return true;
}

bool StringUtil::isWhitespace(const std::string& str) {
    if (str.empty()) {
        return false;
    }
    for (char c : str) {
        if (!std::isspace(c)) {
            return false;
        }
    }
    return true;
}

std::string StringUtil::capitalize(const std::string& str) {
    if (str.empty()) {
        return str;
    }
    std::string result = str;
    result[0] = std::toupper(result[0]);
    return result;
}

std::string StringUtil::decapitalize(const std::string& str) {
    if (str.empty()) {
        return str;
    }
    std::string result = str;
    result[0] = std::tolower(result[0]);
    return result;
}

// PathUtil implementation
std::string PathUtil::join(const std::vector<std::string>& parts) {
    if (parts.empty()) {
        return "";
    }
    
    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        if (!result.empty() && result.back() != '/') {
            result += "/";
        }
        result += parts[i];
    }
    return result;
}

std::string PathUtil::join(const std::string& part1, const std::string& part2) {
    if (part1.empty()) {
        return part2;
    }
    if (part2.empty()) {
        return part1;
    }
    if (part1.back() == '/' || part2.front() == '/') {
        return part1 + part2;
    }
    return part1 + "/" + part2;
}

std::string PathUtil::join(const std::string& part1, const std::string& part2, const std::string& part3) {
    return join(join(part1, part2), part3);
}

std::string PathUtil::normalize(const std::string& path) {
    try {
        return std::filesystem::canonical(path).string();
    } catch (...) {
        return path;
    }
}

std::string PathUtil::resolve(const std::string& path) {
    try {
        return std::filesystem::absolute(path).string();
    } catch (...) {
        return path;
    }
}

std::string PathUtil::relative(const std::string& path, const std::string& base) {
    try {
        return std::filesystem::relative(path, base).string();
    } catch (...) {
        return path;
    }
}

std::string PathUtil::dirname(const std::string& path) {
    return std::filesystem::path(path).parent_path().string();
}

std::string PathUtil::basename(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

std::string PathUtil::extname(const std::string& path) {
    return std::filesystem::path(path).extension().string();
}

std::string PathUtil::stem(const std::string& path) {
    return std::filesystem::path(path).stem().string();
}

bool PathUtil::isAbsolute(const std::string& path) {
    return std::filesystem::path(path).is_absolute();
}

bool PathUtil::isRelative(const std::string& path) {
    return !isAbsolute(path);
}

// TimeUtil implementation
std::string TimeUtil::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    return oss.str();
}

std::string TimeUtil::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

std::string TimeUtil::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

long TimeUtil::getTimestamp() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

// LogUtil implementation
LogUtil::LogLevel LogUtil::current_level = LogUtil::INFO;
std::string LogUtil::log_file = "";
std::string LogUtil::log_format = "[%Y-%m-%d %H:%M:%S] [%l] %m";

void LogUtil::setLogLevel(LogLevel level) {
    current_level = level;
}

void LogUtil::setLogFile(const std::string& filePath) {
    log_file = filePath;
}

void LogUtil::setLogFormat(const std::string& format) {
    log_format = format;
}

void LogUtil::debug(const std::string& message) {
    log(DEBUG, message);
}

void LogUtil::info(const std::string& message) {
    log(INFO, message);
}

void LogUtil::warning(const std::string& message) {
    log(WARNING, message);
}

void LogUtil::error(const std::string& message) {
    log(ERROR, message);
}

void LogUtil::critical(const std::string& message) {
    log(CRITICAL, message);
}

void LogUtil::log(LogLevel level, const std::string& message) {
    if (!isEnabled(level)) {
        return;
    }
    
    std::string formatted_message = formatLogMessage(level, message);
    
    if (!log_file.empty()) {
        std::ofstream file(log_file, std::ios::app);
        if (file.is_open()) {
            file << formatted_message << std::endl;
        }
    } else {
        std::cout << formatted_message << std::endl;
    }
}

bool LogUtil::isEnabled(LogLevel level) {
    return level >= current_level;
}

std::string LogUtil::getLogLevelString(LogLevel level) {
    switch (level) {
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        case CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string LogUtil::formatLogMessage(LogLevel level, const std::string& message) {
    std::string result = log_format;
    result = StringUtil::replaceAll(result, "%Y", TimeUtil::getCurrentDate().substr(0, 4));
    result = StringUtil::replaceAll(result, "%m", TimeUtil::getCurrentDate().substr(5, 2));
    result = StringUtil::replaceAll(result, "%d", TimeUtil::getCurrentDate().substr(8, 2));
    result = StringUtil::replaceAll(result, "%H", TimeUtil::getCurrentTime().substr(0, 2));
    result = StringUtil::replaceAll(result, "%M", TimeUtil::getCurrentTime().substr(3, 2));
    result = StringUtil::replaceAll(result, "%S", TimeUtil::getCurrentTime().substr(6, 2));
    result = StringUtil::replaceAll(result, "%l", getLogLevelString(level));
    result = StringUtil::replaceAll(result, "%m", message);
    return result;
}

// ValidationUtil implementation
bool ValidationUtil::isValidEmail(const std::string& email) {
    std::regex email_regex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, email_regex);
}

bool ValidationUtil::isValidUrl(const std::string& url) {
    std::regex url_regex(R"(https?://[^\s/$.?#].[^\s]*)");
    return std::regex_match(url, url_regex);
}

bool ValidationUtil::isValidIpAddress(const std::string& ip) {
    std::regex ip_regex(R"((\d{1,3}\.){3}\d{1,3})");
    if (!std::regex_match(ip, ip_regex)) {
        return false;
    }
    
    std::vector<std::string> parts = StringUtil::split(ip, ".");
    for (const auto& part : parts) {
        int num = std::stoi(part);
        if (num < 0 || num > 255) {
            return false;
        }
    }
    return true;
}

bool ValidationUtil::isValidDomain(const std::string& domain) {
    std::regex domain_regex(R"([a-zA-Z0-9]([a-zA-Z0-9\-]{0,61}[a-zA-Z0-9])?(\.[a-zA-Z0-9]([a-zA-Z0-9\-]{0,61}[a-zA-Z0-9])?)*)");
    return std::regex_match(domain, domain_regex);
}

bool ValidationUtil::isValidPhoneNumber(const std::string& phone) {
    std::regex phone_regex(R"(\+?1?[-.\s]?\(?[0-9]{3}\)?[-.\s]?[0-9]{3}[-.\s]?[0-9]{4})");
    return std::regex_match(phone, phone_regex);
}

bool ValidationUtil::isValidCreditCard(const std::string& card) {
    // Simple Luhn algorithm check
    std::string clean_card = StringUtil::replaceAll(card, " ", "");
    if (clean_card.length() < 13 || clean_card.length() > 19) {
        return false;
    }
    
    int sum = 0;
    bool alternate = false;
    
    for (int i = clean_card.length() - 1; i >= 0; i--) {
        int digit = clean_card[i] - '0';
        if (alternate) {
            digit *= 2;
            if (digit > 9) {
                digit = (digit % 10) + 1;
            }
        }
        sum += digit;
        alternate = !alternate;
    }
    
    return (sum % 10) == 0;
}

bool ValidationUtil::isValidUuid(const std::string& uuid) {
    std::regex uuid_regex(R"([0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12})");
    return std::regex_match(uuid, uuid_regex);
}

bool ValidationUtil::isValidHex(const std::string& hex) {
    std::regex hex_regex(R"([0-9a-fA-F]+)");
    return std::regex_match(hex, hex_regex);
}

bool ValidationUtil::isValidBase64(const std::string& base64) {
    std::regex base64_regex(R"([A-Za-z0-9+/]*={0,2})");
    return std::regex_match(base64, base64_regex);
}

bool ValidationUtil::isValidRegex(const std::string& pattern) {
    try {
        std::regex regex_pattern(pattern);
        return true;
    } catch (...) {
        return false;
    }
}

bool ValidationUtil::isValidPath(const std::string& path) {
    try {
        std::filesystem::path p(path);
        return true;
    } catch (...) {
        return false;
    }
}

bool ValidationUtil::isValidFilename(const std::string& filename) {
    if (filename.empty() || filename.length() > 255) {
        return false;
    }
    
    std::string invalid_chars = "<>:\"/\\|?*";
    for (char c : invalid_chars) {
        if (filename.find(c) != std::string::npos) {
            return false;
        }
    }
    
    return true;
}

} // namespace CHTL