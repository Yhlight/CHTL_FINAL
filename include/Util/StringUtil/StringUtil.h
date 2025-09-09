#pragma once

#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief 字符串工具类
 * 提供字符串操作功能
 */
class StringUtil {
public:
    StringUtil();
    ~StringUtil();
    
    // 字符串处理
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    
    // 字符串分割和连接
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    std::string join(const std::vector<std::string>& tokens, const std::string& delimiter);
    
    // 字符串替换
    std::string replace(const std::string& str, const std::string& from, const std::string& to);
    
    // 字符串检查
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
    bool contains(const std::string& str, const std::string& substr);
    
    // 字符串转义
    std::string escape(const std::string& str);
    std::string unescape(const std::string& str);

private:
};

} // namespace CHTL