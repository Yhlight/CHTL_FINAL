#ifndef CJMOD_SYNTAX_H
#define CJMOD_SYNTAX_H

#include <string>
#include <vector>
#include <memory>

namespace CJMOD {

/**
 * 语法分析类
 * 负责对 CHTL JS 语法进行解析
 */
class Syntax {
public:
    /**
     * 分析语法，返回参数列表
     * @param syntax 语法字符串
     * @return 解析出的参数列表
     */
    static std::vector<std::string> analyze(const std::string& syntax);
    
    /**
     * 判断是否是 JavaScript 对象
     * @param code 代码字符串
     * @return 是否为 JS 对象
     */
    static bool isObject(const std::string& code);
    
    /**
     * 判断是否是 JavaScript 函数
     * @param code 代码字符串
     * @return 是否为 JS 函数
     */
    static bool isFunction(const std::string& code);
    
    /**
     * 判断是否是 JavaScript 数组
     * @param code 代码字符串
     * @return 是否为 JS 数组
     */
    static bool isArray(const std::string& code);
    
    /**
     * 判断是否是 CHTL JS 函数
     * @param code 代码字符串
     * @return 是否为 CHTL JS 函数
     */
    static bool isCHTLJSFunction(const std::string& code);
    
    /**
     * 验证语法格式
     * @param syntax 语法字符串
     * @return 是否有效
     */
    static bool validateSyntax(const std::string& syntax);
    
    /**
     * 提取占位符
     * @param syntax 语法字符串
     * @return 占位符列表
     */
    static std::vector<std::string> extractPlaceholders(const std::string& syntax);
    
    /**
     * 检查语法完整性
     * @param syntax 语法字符串
     * @return 是否完整
     */
    static bool isComplete(const std::string& syntax);

private:
    /**
     * 跳过空白字符
     * @param str 字符串
     * @param pos 位置引用
     */
    static void skipWhitespace(const std::string& str, size_t& pos);
    
    /**
     * 跳过注释
     * @param str 字符串
     * @param pos 位置引用
     * @return 是否跳过了注释
     */
    static bool skipComment(const std::string& str, size_t& pos);
    
    /**
     * 跳过字符串
     * @param str 字符串
     * @param pos 位置引用
     * @return 是否跳过了字符串
     */
    static bool skipString(const std::string& str, size_t& pos);
    
    /**
     * 检查括号匹配
     * @param str 字符串
     * @return 是否匹配
     */
    static bool checkBracketMatch(const std::string& str);
    
    /**
     * 提取标识符
     * @param str 字符串
     * @param pos 位置引用
     * @return 标识符
     */
    static std::string extractIdentifier(const std::string& str, size_t& pos);
    
    /**
     * 检查是否为占位符
     * @param str 字符串
     * @param pos 位置
     * @return 是否为占位符
     */
    static bool isPlaceholder(const std::string& str, size_t pos);
    
    /**
     * 提取占位符内容
     * @param str 字符串
     * @param pos 位置引用
     * @return 占位符内容
     */
    static std::string extractPlaceholder(const std::string& str, size_t& pos);
};

} // namespace CJMOD

#endif // CJMOD_SYNTAX_H