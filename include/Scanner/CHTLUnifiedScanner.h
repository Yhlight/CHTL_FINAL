#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 代码片段类型枚举
 */
enum class CodeFragmentType {
    CHTL,           // CHTL代码片段
    CHTL_JS,        // CHTL JS代码片段
    CSS,            // CSS代码片段
    JS,             // JavaScript代码片段
    HTML,           // HTML代码片段
    UNKNOWN         // 未知类型
};

/**
 * @brief 代码片段结构
 */
struct CodeFragment {
    CodeFragmentType type;
    std::string content;
    size_t startLine;
    size_t endLine;
    size_t startColumn;
    size_t endColumn;
    std::string originalContent; // 原始内容，用于错误报告
    
    CodeFragment(CodeFragmentType t = CodeFragmentType::UNKNOWN,
                 const std::string& c = "",
                 size_t sl = 0, size_t el = 0,
                 size_t sc = 0, size_t ec = 0,
                 const std::string& orig = "")
        : type(t), content(c), startLine(sl), endLine(el),
          startColumn(sc), endColumn(ec), originalContent(orig) {}
};

/**
 * @brief CHTL统一扫描器
 * 
 * 负责将混合的CHTL代码分离成不同类型的代码片段
 * 支持CHTL、CHTL JS、CSS、JavaScript的精确分离
 */
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner() = default;

    /**
     * @brief 扫描输入代码，分离不同类型的代码片段
     * @param input 输入代码
     * @return 代码片段列表
     */
    std::vector<CodeFragment> scan(const std::string& input);
    
    /**
     * @brief 从文件扫描
     * @param filePath 文件路径
     * @return 代码片段列表
     */
    std::vector<CodeFragment> scanFile(const std::string& filePath);

    /**
     * @brief 设置扫描选项
     * @param strictMode 严格模式，更严格的语法检查
     * @param preserveComments 保留注释
     */
    void setScanOptions(bool strictMode, bool preserveComments);

    /**
     * @brief 获取扫描错误
     * @return 错误信息列表
     */
    const std::vector<std::string>& getErrors() const;
    
    /**
     * @brief 是否有错误
     * @return 是否有错误
     */
    bool hasErrors() const;

    /**
     * @brief 重置扫描器状态
     */
    void reset();

private:
    // 扫描状态
    std::string m_input;
    size_t m_position;
    size_t m_line;
    size_t m_column;
    std::vector<std::string> m_errors;
    
    // 扫描选项
    bool m_strictMode;
    bool m_preserveComments;
    
    // 占位符管理
    std::unordered_map<std::string, std::string> m_placeholders;
    int m_placeholderCounter;
    
    // 扫描函数
    CodeFragment scanCHTL();
    CodeFragment scanCHTLJS();
    CodeFragment scanCSS();
    CodeFragment scanJavaScript();
    CodeFragment scanHTML();
    
    // 辅助函数
    bool isAtEnd() const;
    char currentChar() const;
    char peekChar() const;
    char peekChar(int offset) const;
    void advance();
    void advance(int count);
    
    // 跳过函数
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    void skipGeneratorComment();
    
    // 识别函数
    bool isCHTLKeyword(const std::string& word) const;
    bool isCHTLJSKeyword(const std::string& word) const;
    bool isCSSKeyword(const std::string& word) const;
    bool isJSKeyword(const std::string& word) const;
    
    // 边界检测
    bool isCHTLBoundary() const;
    bool isCHTLJSBoundary() const;
    bool isCSSBoundary() const;
    bool isJSBoundary() const;
    bool isHTMLBoundary() const;
    
    // 占位符处理
    std::string createPlaceholder(const std::string& content);
    std::string restorePlaceholder(const std::string& placeholder);
    void processPlaceholders(std::string& content);
    
    // 字符串处理
    std::string readString(char delimiter);
    std::string readTemplateString();
    std::string readRegex();
    
    // 注释处理
    void skipComments();
    bool isCommentStart() const;
    bool isLineComment() const;
    bool isBlockComment() const;
    bool isGeneratorComment() const;
    
    // 错误处理
    void addError(const std::string& message);
    void addErrorAtPosition(const std::string& message, size_t line, size_t column);
    
    // 位置管理
    void updatePosition();
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    
    // 工具函数
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isOperator(char c) const;
    bool isPunctuation(char c) const;
    
    // 关键字表
    void initializeKeywordTables();
    std::unordered_map<std::string, bool> m_chtlKeywords;
    std::unordered_map<std::string, bool> m_chtljsKeywords;
    std::unordered_map<std::string, bool> m_cssKeywords;
    std::unordered_map<std::string, bool> m_jsKeywords;
};

} // namespace CHTL