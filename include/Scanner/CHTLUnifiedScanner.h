#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief 代码片段类型枚举
 */
enum class CodeFragmentType {
    CHTL,           // CHTL代码片段
    CHTL_JS,        // CHTL JS代码片段
    CSS,            // CSS代码片段
    JAVASCRIPT      // JavaScript代码片段
};

/**
 * @brief 代码片段
 */
struct CodeFragment {
    CodeFragmentType type;
    std::string content;
    size_t startLine;
    size_t endLine;
    size_t startColumn;
    size_t endColumn;
    
    CodeFragment(CodeFragmentType t, const std::string& c, size_t sl = 0, size_t el = 0, size_t sc = 0, size_t ec = 0)
        : type(t), content(c), startLine(sl), endLine(el), startColumn(sc), endColumn(ec) {}
};

/**
 * @brief CHTL统一扫描器
 * 负责将混合代码分离为不同类型的代码片段
 */
class CHTLUnifiedScanner {
public:
    CHTLUnifiedScanner();
    ~CHTLUnifiedScanner();
    
    // 扫描源代码，分离不同类型的代码片段
    std::vector<CodeFragment> scan(const std::string& source);
    
    // 设置扫描选项
    void setDebugMode(bool enabled);
    void setStrictMode(bool enabled);
    void setPreserveComments(bool enabled);
    
    // 获取扫描统计信息
    size_t getFragmentCount() const;
    size_t getCHTLFragmentCount() const;
    size_t getCHTLJSFragmentCount() const;
    size_t getCSSFragmentCount() const;
    size_t getJavaScriptFragmentCount() const;

private:
    bool m_debugMode;
    bool m_strictMode;
    bool m_preserveComments;
    size_t m_fragmentCount;
    size_t m_chtlFragmentCount;
    size_t m_chtljsFragmentCount;
    size_t m_cssFragmentCount;
    size_t m_javascriptFragmentCount;
    
    // 扫描状态
    struct ScanState {
        size_t position;
        size_t line;
        size_t column;
        bool inString;
        bool inComment;
        bool inCHTLBlock;
        bool inCHTLJSBlock;
        bool inCSSBlock;
        bool inJavaScriptBlock;
        int braceLevel;
        int bracketLevel;
        int parenLevel;
        std::string currentFragment;
        CodeFragmentType currentType;
    };
    
    // 主要扫描方法
    std::vector<CodeFragment> scanSource(const std::string& source);
    void scanFragment(ScanState& state, const std::string& source, std::vector<CodeFragment>& fragments);
    
    // CHTL代码识别
    bool isCHTLKeyword(const std::string& word) const;
    bool isCHTLBlockStart(const std::string& source, size_t pos) const;
    bool isCHTLBlockEnd(const std::string& source, size_t pos) const;
    
    // CHTL JS代码识别
    bool isCHTLJSKeyword(const std::string& word) const;
    bool isCHTLJSBlockStart(const std::string& source, size_t pos) const;
    bool isCHTLJSBlockEnd(const std::string& source, size_t pos) const;
    
    // CSS代码识别
    bool isCSSBlockStart(const std::string& source, size_t pos) const;
    bool isCSSBlockEnd(const std::string& source, size_t pos) const;
    
    // JavaScript代码识别
    bool isJavaScriptBlockStart(const std::string& source, size_t pos) const;
    bool isJavaScriptBlockEnd(const std::string& source, size_t pos) const;
    
    // 占位符机制
    std::string replaceWithPlaceholders(const std::string& source, CodeFragmentType type);
    std::string restoreFromPlaceholders(const std::string& source);
    
    // 字符串处理
    bool isInString(const std::string& source, size_t pos) const;
    bool isInComment(const std::string& source, size_t pos) const;
    bool isInBlockComment(const std::string& source, size_t pos) const;
    bool isInLineComment(const std::string& source, size_t pos) const;
    
    // 括号匹配
    bool isBalanced(const std::string& source, size_t start, size_t end) const;
    int findMatchingBrace(const std::string& source, size_t pos) const;
    int findMatchingBracket(const std::string& source, size_t pos) const;
    int findMatchingParen(const std::string& source, size_t pos) const;
    
    // 边界检测
    bool isCodeBoundary(const std::string& source, size_t pos) const;
    bool isFragmentBoundary(const std::string& source, size_t pos) const;
    
    // 宽判和严判
    bool isWideMode(const std::string& source, size_t pos) const;
    bool isStrictMode(const std::string& source, size_t pos) const;
    
    // 辅助方法
    char getChar(const std::string& source, size_t pos) const;
    std::string getWord(const std::string& source, size_t pos) const;
    bool isWhitespace(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isOperator(char c) const;
    
    // 错误处理
    void addError(const std::string& error);
    const std::vector<std::string>& getErrors() const;
    bool hasErrors() const;
    
    std::vector<std::string> m_errors;
};

} // namespace CHTL