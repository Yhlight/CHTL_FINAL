#ifndef CHTL_UTILITIES_H
#define CHTL_UTILITIES_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <regex>

namespace CHTL {

// FileSystem utility
class FileSystem {
public:
    static bool exists(const std::string& path);
    static bool isFile(const std::string& path);
    static bool isDirectory(const std::string& path);
    static bool createDirectory(const std::string& path);
    static bool createDirectories(const std::string& path);
    static bool remove(const std::string& path);
    static bool removeAll(const std::string& path);
    static bool copy(const std::string& source, const std::string& destination);
    static bool move(const std::string& source, const std::string& destination);
    static size_t getFileSize(const std::string& path);
    static std::string getFileName(const std::string& path);
    static std::string getDirectory(const std::string& path);
    static std::string getExtension(const std::string& path);
    static std::string getAbsolutePath(const std::string& path);
    static std::string getRelativePath(const std::string& path, const std::string& base);
    static std::vector<std::string> listFiles(const std::string& directory);
    static std::vector<std::string> listDirectories(const std::string& directory);
    static std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern);
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static bool appendFile(const std::string& path, const std::string& content);
};

// StringUtil utility
class StringUtil {
public:
    static std::string trim(const std::string& str);
    static std::string ltrim(const std::string& str);
    static std::string rtrim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static bool contains(const std::string& str, const std::string& substring);
    static std::string substring(const std::string& str, size_t start, size_t length = std::string::npos);
    static std::string padLeft(const std::string& str, size_t width, char pad = ' ');
    static std::string padRight(const std::string& str, size_t width, char pad = ' ');
    static std::string padCenter(const std::string& str, size_t width, char pad = ' ');
    static std::string escape(const std::string& str);
    static std::string unescape(const std::string& str);
    static std::string escapeHtml(const std::string& str);
    static std::string unescapeHtml(const std::string& str);
    static std::string escapeJson(const std::string& str);
    static std::string unescapeJson(const std::string& str);
    static std::string escapeXml(const std::string& str);
    static std::string unescapeXml(const std::string& str);
    static bool isNumeric(const std::string& str);
    static bool isAlpha(const std::string& str);
    static bool isAlphaNumeric(const std::string& str);
    static bool isWhitespace(const std::string& str);
    static std::string capitalize(const std::string& str);
    static std::string decapitalize(const std::string& str);
    static std::string camelCase(const std::string& str);
    static std::string snakeCase(const std::string& str);
    static std::string kebabCase(const std::string& str);
    static std::string pascalCase(const std::string& str);
};

// ZipUtil utility
class ZipUtil {
public:
    static bool compress(const std::string& source, const std::string& destination);
    static bool compressDirectory(const std::string& directory, const std::string& destination);
    static bool extract(const std::string& source, const std::string& destination);
    static std::vector<std::string> listFiles(const std::string& zipFile);
    static bool addFile(const std::string& zipFile, const std::string& filePath, const std::string& archivePath);
    static bool removeFile(const std::string& zipFile, const std::string& archivePath);
    static bool isZipFile(const std::string& path);
    static size_t getZipSize(const std::string& zipFile);
    static std::string getZipComment(const std::string& zipFile);
    static bool setZipComment(const std::string& zipFile, const std::string& comment);
};

// PathUtil utility
class PathUtil {
public:
    static std::string join(const std::vector<std::string>& parts);
    static std::string join(const std::string& part1, const std::string& part2);
    static std::string join(const std::string& part1, const std::string& part2, const std::string& part3);
    static std::string normalize(const std::string& path);
    static std::string resolve(const std::string& path);
    static std::string relative(const std::string& path, const std::string& base);
    static std::string dirname(const std::string& path);
    static std::string basename(const std::string& path);
    static std::string extname(const std::string& path);
    static std::string stem(const std::string& path);
    static bool isAbsolute(const std::string& path);
    static bool isRelative(const std::string& path);
    static std::string toAbsolute(const std::string& path);
    static std::string toRelative(const std::string& path, const std::string& base);
    static std::vector<std::string> split(const std::string& path);
    static std::string commonPrefix(const std::vector<std::string>& paths);
    static std::string commonSuffix(const std::vector<std::string>& paths);
};

// RegexUtil utility
class RegexUtil {
public:
    static bool match(const std::string& str, const std::string& pattern);
    static std::vector<std::string> findAll(const std::string& str, const std::string& pattern);
    static std::string replace(const std::string& str, const std::string& pattern, const std::string& replacement);
    static std::string replaceAll(const std::string& str, const std::string& pattern, const std::string& replacement);
    static std::vector<std::string> split(const std::string& str, const std::string& pattern);
    static bool isValidPattern(const std::string& pattern);
    static std::string escape(const std::string& str);
    static std::string unescape(const std::string& str);
    static std::string compile(const std::string& pattern);
    static bool test(const std::string& str, const std::string& pattern);
    static std::string extract(const std::string& str, const std::string& pattern);
    static std::vector<std::string> extractAll(const std::string& str, const std::string& pattern);
};

// HashUtil utility
class HashUtil {
public:
    static std::string md5(const std::string& str);
    static std::string sha1(const std::string& str);
    static std::string sha256(const std::string& str);
    static std::string sha512(const std::string& str);
    static std::string crc32(const std::string& str);
    static std::string adler32(const std::string& str);
    static std::string hash(const std::string& str, const std::string& algorithm);
    static bool verify(const std::string& str, const std::string& hash, const std::string& algorithm);
    static std::string hmac(const std::string& str, const std::string& key, const std::string& algorithm);
    static std::string pbkdf2(const std::string& password, const std::string& salt, int iterations, int keyLength);
    static std::string bcrypt(const std::string& password, int rounds = 10);
    static bool verifyBcrypt(const std::string& password, const std::string& hash);
};

// TimeUtil utility
class TimeUtil {
public:
    static std::string getCurrentTime();
    static std::string getCurrentDate();
    static std::string getCurrentDateTime();
    static std::string formatTime(const std::string& format);
    static std::string formatDate(const std::string& format);
    static std::string formatDateTime(const std::string& format);
    static long getTimestamp();
    static std::string timestampToString(long timestamp);
    static long stringToTimestamp(const std::string& str);
    static std::string getTimeZone();
    static std::string convertTimeZone(const std::string& time, const std::string& fromTz, const std::string& toTz);
    static bool isLeapYear(int year);
    static int getDaysInMonth(int year, int month);
    static int getDayOfWeek(int year, int month, int day);
    static std::string getDayName(int dayOfWeek);
    static std::string getMonthName(int month);
};

// ConfigUtil utility
class ConfigUtil {
public:
    static std::map<std::string, std::string> loadConfig(const std::string& filePath);
    static bool saveConfig(const std::map<std::string, std::string>& config, const std::string& filePath);
    static std::string getConfigValue(const std::map<std::string, std::string>& config, const std::string& key);
    static std::string getConfigValue(const std::map<std::string, std::string>& config, const std::string& key, const std::string& defaultValue);
    static bool setConfigValue(std::map<std::string, std::string>& config, const std::string& key, const std::string& value);
    static bool removeConfigValue(std::map<std::string, std::string>& config, const std::string& key);
    static bool hasConfigValue(const std::map<std::string, std::string>& config, const std::string& key);
    static std::vector<std::string> getConfigKeys(const std::map<std::string, std::string>& config);
    static std::vector<std::string> getConfigValues(const std::map<std::string, std::string>& config);
    static void clearConfig(std::map<std::string, std::string>& config);
    static size_t getConfigSize(const std::map<std::string, std::string>& config);
    static bool isEmpty(const std::map<std::string, std::string>& config);
};

// LogUtil utility
class LogUtil {
public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };
    
    static void setLogLevel(LogLevel level);
    static void setLogFile(const std::string& filePath);
    static void setLogFormat(const std::string& format);
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warning(const std::string& message);
    static void error(const std::string& message);
    static void critical(const std::string& message);
    static void log(LogLevel level, const std::string& message);
    static void log(LogLevel level, const std::string& message, const std::string& file, int line);
    static void log(LogLevel level, const std::string& message, const std::string& file, int line, const std::string& function);
    static void flush();
    static void close();
    static bool isEnabled(LogLevel level);
    static std::string getLogLevelString(LogLevel level);
    static LogLevel getLogLevelFromString(const std::string& level);
    
private:
    static LogLevel current_level;
    static std::string log_file;
    static std::string log_format;
    static std::string formatLogMessage(LogLevel level, const std::string& message);
};

// ValidationUtil utility
class ValidationUtil {
public:
    static bool isValidEmail(const std::string& email);
    static bool isValidUrl(const std::string& url);
    static bool isValidIpAddress(const std::string& ip);
    static bool isValidDomain(const std::string& domain);
    static bool isValidPhoneNumber(const std::string& phone);
    static bool isValidCreditCard(const std::string& card);
    static bool isValidPostalCode(const std::string& code);
    static bool isValidSsn(const std::string& ssn);
    static bool isValidUuid(const std::string& uuid);
    static bool isValidHex(const std::string& hex);
    static bool isValidBase64(const std::string& base64);
    static bool isValidJson(const std::string& json);
    static bool isValidXml(const std::string& xml);
    static bool isValidCsv(const std::string& csv);
    static bool isValidYaml(const std::string& yaml);
    static bool isValidToml(const std::string& toml);
    static bool isValidIni(const std::string& ini);
    static bool isValidRegex(const std::string& pattern);
    static bool isValidPath(const std::string& path);
    static bool isValidFilename(const std::string& filename);
    static bool isValidDirectory(const std::string& directory);
    static bool isValidFile(const std::string& file);
    static bool isValidExtension(const std::string& extension);
    static bool isValidMimeType(const std::string& mimeType);
    static bool isValidCharset(const std::string& charset);
    static bool isValidEncoding(const std::string& encoding);
    static bool isValidLanguage(const std::string& language);
    static bool isValidCountry(const std::string& country);
    static bool isValidCurrency(const std::string& currency);
    static bool isValidTimeZone(const std::string& timezone);
    static bool isValidDateFormat(const std::string& format);
    static bool isValidTimeFormat(const std::string& format);
    static bool isValidDateTimeFormat(const std::string& format);
};

} // namespace CHTL

#endif // CHTL_UTILITIES_H