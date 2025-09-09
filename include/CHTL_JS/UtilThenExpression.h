#ifndef CHTL_JS_UTIL_THEN_EXPRESSION_H
#define CHTL_JS_UTIL_THEN_EXPRESSION_H

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace CHTL_JS {

/**
 * util...then 表达式类
 * 支持条件变化监听：util 表达式 -> change { 条件变化时 } -> then { 条件完成时 }
 */
class UtilThenExpression {
public:
    /**
     * 构造函数
     * @param expression 条件表达式
     */
    explicit UtilThenExpression(const std::string& expression);
    
    /**
     * 析构函数
     */
    ~UtilThenExpression() = default;
    
    /**
     * 设置变化时的回调
     * @param callback 变化回调函数
     */
    void setChangeCallback(const std::string& callback);
    
    /**
     * 设置完成时的回调
     * @param callback 完成回调函数
     */
    void setThenCallback(const std::string& callback);
    
    /**
     * 设置变化时的回调（多行）
     * @param callbacks 变化回调函数列表
     */
    void setChangeCallbacks(const std::vector<std::string>& callbacks);
    
    /**
     * 设置完成时的回调（多行）
     * @param callbacks 完成回调函数列表
     */
    void setThenCallbacks(const std::vector<std::string>& callbacks);
    
    /**
     * 获取条件表达式
     * @return 条件表达式
     */
    const std::string& getExpression() const { return m_expression; }
    
    /**
     * 获取变化回调
     * @return 变化回调
     */
    const std::string& getChangeCallback() const { return m_changeCallback; }
    
    /**
     * 获取完成回调
     * @return 完成回调
     */
    const std::string& getThenCallback() const { return m_thenCallback; }
    
    /**
     * 检查是否为单行语句
     * @return 是否为单行语句
     */
    bool isSingleLine() const { return m_isSingleLine; }
    
    /**
     * 生成 JavaScript 代码
     * @return 生成的 JS 代码
     */
    std::string generateJavaScript() const;
    
    /**
     * 验证表达式语法
     * @return 是否有效
     */
    bool validate() const;
    
    /**
     * 设置调试模式
     * @param enabled 是否启用调试
     */
    void setDebugMode(bool enabled);

private:
    std::string m_expression;
    std::string m_changeCallback;
    std::string m_thenCallback;
    std::vector<std::string> m_changeCallbacks;
    std::vector<std::string> m_thenCallbacks;
    bool m_isSingleLine;
    bool m_debugMode;
    
    /**
     * 解析表达式
     */
    void parseExpression();
    
    /**
     * 生成条件监听代码
     * @return 条件监听代码
     */
    std::string generateConditionListener() const;
    
    /**
     * 生成变化处理代码
     * @return 变化处理代码
     */
    std::string generateChangeHandler() const;
    
    /**
     * 生成完成处理代码
     * @return 完成处理代码
     */
    std::string generateThenHandler() const;
    
    /**
     * 生成变量声明代码
     * @return 变量声明代码
     */
    std::string generateVariableDeclaration() const;
    
    /**
     * 调试输出
     * @param message 调试消息
     */
    void debugOutput(const std::string& message) const;
};

/**
 * util...then 表达式解析器
 */
class UtilThenParser {
public:
    /**
     * 解析 util...then 表达式
     * @param code 代码字符串
     * @return 解析结果
     */
    static std::vector<UtilThenExpression> parse(const std::string& code);
    
    /**
     * 检查是否为 util...then 表达式
     * @param code 代码字符串
     * @return 是否为 util...then 表达式
     */
    static bool isUtilThenExpression(const std::string& code);
    
    /**
     * 提取表达式部分
     * @param code 代码字符串
     * @return 表达式字符串
     */
    static std::string extractExpression(const std::string& code);
    
    /**
     * 提取 change 部分
     * @param code 代码字符串
     * @return change 代码字符串
     */
    static std::string extractChange(const std::string& code);
    
    /**
     * 提取 then 部分
     * @param code 代码字符串
     * @return then 代码字符串
     */
    static std::string extractThen(const std::string& code);

private:
    /**
     * 查找关键字位置
     * @param code 代码字符串
     * @param keyword 关键字
     * @param startPos 开始位置
     * @return 关键字位置
     */
    static size_t findKeyword(const std::string& code, const std::string& keyword, size_t startPos);
    
    /**
     * 提取代码块
     * @param code 代码字符串
     * @param startPos 开始位置
     * @param endPos 结束位置
     * @return 代码块字符串
     */
    static std::string extractCodeBlock(const std::string& code, size_t startPos, size_t endPos);
    
    /**
     * 跳过空白字符
     * @param code 代码字符串
     * @param pos 位置引用
     */
    static void skipWhitespace(const std::string& code, size_t& pos);
    
    /**
     * 查找匹配的大括号
     * @param code 代码字符串
     * @param startPos 开始位置
     * @return 匹配的大括号位置
     */
    static size_t findMatchingBrace(const std::string& code, size_t startPos);
};

} // namespace CHTL_JS

#endif // CHTL_JS_UTIL_THEN_EXPRESSION_H