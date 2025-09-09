#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace CHTL {

/**
 * 代码片段类型枚举
 */
enum class CodeFragmentType {
    CHTL,           // CHTL 代码片段
    CHTL_JS,        // CHTL JS 代码片段
    CSS,            // CSS 代码片段
    JS,             // JavaScript 代码片段
    UNKNOWN         // 未知类型
};

/**
 * 代码片段结构
 */
struct CodeFragment {
    CodeFragmentType type;
    std::string content;
    size_t startPos;
    size_t endPos;
    int line;
    int column;
    
    CodeFragment(CodeFragmentType t, const std::string& c, size_t start, size_t end, int l = 0, int col = 0)
        : type(t), content(c), startPos(start), endPos(end), line(l), column(col) {}
};

/**
 * 扫描模式枚举
 */
enum class ScanMode {
    WIDE_JUDGMENT,      // 宽判模式 - 用于处理大块 CHTL 代码
    STRICT_JUDGMENT     // 严判模式 - 用于处理 CHTL JS 和 JS 混合代码
};

/**
 * 统一扫描器类
 * 负责将 CHTL 源代码分离成不同类型的代码片段
 */
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner();

    /**
     * 扫描源代码，分离不同类型的代码片段
     * @param sourceCode 源代码字符串
     * @return 代码片段列表
     */
    std::vector<CodeFragment> scan(const std::string& sourceCode);

    /**
     * 设置占位符标识符
     * @param placeholder 占位符字符串
     */
    void setPlaceholder(const std::string& placeholder);

    /**
     * 设置扫描模式
     * @param mode 扫描模式
     */
    void setScanMode(ScanMode mode);

    /**
     * 启用/禁用调试模式
     * @param enabled 是否启用调试
     */
    void setDebugMode(bool enabled);

private:
    std::string m_placeholder;
    ScanMode m_scanMode;
    bool m_debugMode;
    
    // 源代码相关
    std::string m_sourceCode;
    size_t m_currentPos;
    int m_currentLine;
    int m_currentColumn;
    
    // 状态管理
    bool m_inString;
    bool m_inComment;
    bool m_inCHTLBlock;
    bool m_inCHTLJSBlock;
    bool m_inStyleBlock;
    bool m_inScriptBlock;
    
    // 括号和块计数器
    int m_braceCount;
    int m_parenCount;
    int m_bracketCount;
    
    /**
     * 重置扫描器状态
     */
    void resetState();
    
    /**
     * 跳过空白字符
     */
    void skipWhitespace();
    
    /**
     * 跳过注释
     * @return 是否跳过了注释
     */
    bool skipComment();
    
    /**
     * 跳过字符串
     * @return 是否跳过了字符串
     */
    bool skipString();
    
    /**
     * 检查当前位置是否为 CHTL 语法
     * @return 是否为 CHTL 语法
     */
    bool isCHTLSyntax();
    
    /**
     * 检查当前位置是否为 CHTL JS 语法
     * @return 是否为 CHTL JS 语法
     */
    bool isCHTLJSSyntax();
    
    /**
     * 检查当前位置是否为 CSS 语法
     * @return 是否为 CSS 语法
     */
    bool isCSSSyntax();
    
    /**
     * 检查当前位置是否为 JS 语法
     * @return 是否为 JS 语法
     */
    bool isJSSyntax();
    
    /**
     * 扫描 CHTL 代码块
     * @return CHTL 代码片段
     */
    CodeFragment scanCHTLBlock();
    
    /**
     * 扫描 CHTL JS 代码块
     * @return CHTL JS 代码片段
     */
    CodeFragment scanCHTLJSBlock();
    
    /**
     * 扫描 CSS 代码块
     * @return CSS 代码片段
     */
    CodeFragment scanCSSBlock();
    
    /**
     * 扫描 JS 代码块
     * @return JS 代码片段
     */
    CodeFragment scanJSBlock();
    
    /**
     * 使用占位符机制分离 CHTL JS 和 JS 代码
     * @param content 原始内容
     * @return 处理后的内容
     */
    std::string applyPlaceholderMechanism(const std::string& content);
    
    /**
     * 双指针扫描算法
     * @param content 要扫描的内容
     * @param keyword 关键字
     * @return 扫描结果
     */
    std::vector<std::string> doublePointerScan(const std::string& content, const std::string& keyword);
    
    /**
     * 前置截取处理
     * @param content 原始内容
     * @return 处理后的内容
     */
    std::string preprocessContent(const std::string& content);
    
    /**
     * 智能扩增扫描范围
     * @param startPos 开始位置
     * @param endPos 结束位置
     * @return 调整后的结束位置
     */
    size_t smartExpandRange(size_t startPos, size_t endPos);
    
    /**
     * 检查语法边界
     * @param pos 位置
     * @return 是否为语法边界
     */
    bool isSyntaxBoundary(size_t pos);
    
    /**
     * 获取下一个字符
     * @return 下一个字符，如果到达末尾返回 '\0'
     */
    char peekNext();
    
    /**
     * 获取指定偏移的字符
     * @param offset 偏移量
     * @return 字符，如果越界返回 '\0'
     */
    char peekAt(size_t offset);
    
    /**
     * 移动当前位置
     * @param steps 移动步数
     */
    void advance(size_t steps = 1);
    
    /**
     * 调试输出
     * @param message 调试消息
     */
    void debugOutput(const std::string& message);
    
    /**
     * 检查是否为块开始标记
     * @param pos 位置
     * @return 是否为块开始
     */
    bool isBlockStart(size_t pos);
    
    /**
     * 检查是否为块结束标记
     * @param pos 位置
     * @return 是否为块结束
     */
    bool isBlockEnd(size_t pos);
    
    /**
     * 查找匹配的结束标记
     * @param startPos 开始位置
     * @param startChar 开始字符
     * @param endChar 结束字符
     * @return 匹配的结束位置，如果未找到返回 std::string::npos
     */
    size_t findMatchingEnd(size_t startPos, char startChar, char endChar);
    
    /**
     * 处理嵌套结构
     * @param startPos 开始位置
     * @return 处理后的结束位置
     */
    size_t handleNestedStructure(size_t startPos);
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H