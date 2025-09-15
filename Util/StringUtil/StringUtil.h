#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

/**
 * 字符串工具类
 * 提供常用的字符串操作功能
 */
class StringUtil {
public:
    // 基本操作
    static std::string trim(const std::string& str);
    static std::string trimLeft(const std::string& str);
    static std::string trimRight(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string capitalize(const std::string& str);
    static std::string reverse(const std::string& str);
    
    // 查找和替换
    static bool contains(const std::string& str, const std::string& substr);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static size_t find(const std::string& str, const std::string& substr, size_t pos = 0);
    static size_t findLast(const std::string& str, const std::string& substr);
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceRegex(const std::string& str, const std::string& pattern, const std::string& replacement);
    
    // 分割和连接
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    static std::vector<std::string> splitLines(const std::string& str);
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    static std::string join(const std::vector<std::string>& strings, char delimiter);
    
    // 格式化
    static std::string format(const std::string& format, const std::vector<std::string>& args);
    static std::string padLeft(const std::string& str, size_t width, char padding = ' ');
    static std::string padRight(const std::string& str, size_t width, char padding = ' ');
    static std::string padCenter(const std::string& str, size_t width, char padding = ' ');
    
    // 类型转换
    static int toInt(const std::string& str, int defaultValue = 0);
    static long toLong(const std::string& str, long defaultValue = 0);
    static double toDouble(const std::string& str, double defaultValue = 0.0);
    static bool toBool(const std::string& str, bool defaultValue = false);
    static std::string toString(int value);
    static std::string toString(long value);
    static std::string toString(double value);
    static std::string toString(bool value);
    
    // 验证
    static bool isNumeric(const std::string& str);
    static bool isAlpha(const std::string& str);
    static bool isAlphaNumeric(const std::string& str);
    static bool isWhitespace(const std::string& str);
    static bool isEmpty(const std::string& str);
    static bool isBlank(const std::string& str);
    
    // 编码转换
    static std::string urlEncode(const std::string& str);
    static std::string urlDecode(const std::string& str);
    static std::string htmlEncode(const std::string& str);
    static std::string htmlDecode(const std::string& str);
    static std::string base64Encode(const std::string& str);
    static std::string base64Decode(const std::string& str);
    
    // 正则表达式
    static bool matches(const std::string& str, const std::string& pattern);
    static std::vector<std::string> findAll(const std::string& str, const std::string& pattern);
    static std::string extract(const std::string& str, const std::string& pattern);
    
    // 字符串操作
    static std::string substring(const std::string& str, size_t start, size_t length = std::string::npos);
    static std::string insert(const std::string& str, size_t pos, const std::string& substr);
    static std::string remove(const std::string& str, size_t start, size_t length);
    static std::string repeat(const std::string& str, int count);
    
    // 比较
    static int compare(const std::string& str1, const std::string& str2, bool caseSensitive = true);
    static bool equals(const std::string& str1, const std::string& str2, bool caseSensitive = true);
    static bool equalsIgnoreCase(const std::string& str1, const std::string& str2);
    
    // 统计
    static size_t count(const std::string& str, const std::string& substr);
    static size_t count(const std::string& str, char c);
    static size_t length(const std::string& str);
    static size_t length(const std::wstring& str);
    
    // 特殊处理
    static std::string escape(const std::string& str, const std::string& chars);
    static std::string unescape(const std::string& str, const std::string& chars);
    static std::string normalize(const std::string& str);
    static std::string slugify(const std::string& str);
    
    // 模板处理
    static std::string interpolate(const std::string& template_str, const std::map<std::string, std::string>& variables);
    static std::vector<std::string> extractVariables(const std::string& template_str);
    
private:
    static std::string base64Chars;
    static std::string urlSafeChars;
};

} // namespace CHTL

#endif // STRING_UTIL_H