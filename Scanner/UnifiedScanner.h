#ifndef UNIFIED_SCANNER_H
#define UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

enum class CodeType {
    CHTL,           // CHTL代码
    CHTLJS,         // CHTL JS代码
    CSS,            // CSS代码
    JS,             // JavaScript代码
    HTML,           // HTML代码
    UNKNOWN         // 未知类型
};

struct CodeFragment {
    CodeType type;
    std::string content;
    size_t startLine;
    size_t endLine;
    size_t startColumn;
    size_t endColumn;
    std::string placeholder;  // 占位符，用于替换
    
    CodeFragment(CodeType t = CodeType::UNKNOWN, const std::string& c = "")
        : type(t), content(c), startLine(0), endLine(0), startColumn(0), endColumn(0) {}
};

class UnifiedScanner {
private:
    std::string source;
    std::vector<CodeFragment> fragments;
    std::unordered_map<std::string, CodeFragment> placeholders;
    size_t placeholderCounter;
    
    // 扫描状态
    size_t position;
    size_t line;
    size_t column;
    
    // 配置选项
    bool debugMode;
    bool preserveComments;
    bool enablePlaceholders;
    
    // 扫描方法
    void scanSource();
    CodeFragment scanCHTL();
    CodeFragment scanCHTLJS();
    CodeFragment scanCSS();
    CodeFragment scanJS();
    CodeFragment scanHTML();
    
    // 辅助方法
    char current() const;
    char peek(size_t offset = 1) const;
    void advance();
    bool isAtEnd() const;
    void skipWhitespace();
    void skipComment();
    void skipLineComment();
    void skipBlockComment();
    
    // 边界检测
    bool isCHTLBoundary() const;
    bool isCHTLJSBoundary() const;
    bool isCSSBoundary() const;
    bool isJSBoundary() const;
    bool isHTMLBoundary() const;
    
    // 占位符管理
    std::string generatePlaceholder();
    void addPlaceholder(const std::string& placeholder, const CodeFragment& fragment);
    CodeFragment getPlaceholder(const std::string& placeholder) const;
    bool hasPlaceholder(const std::string& placeholder) const;
    
    // 工具方法
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isDigit(char c) const;
    
    void error(const std::string& message) const;
    
public:
    UnifiedScanner();
    ~UnifiedScanner() = default;
    
    // 主要扫描方法
    std::vector<CodeFragment> scan(const std::string& source);
    std::vector<CodeFragment> scanFile(const std::string& filePath);
    
    // 配置方法
    void setDebugMode(bool debug);
    void setPreserveComments(bool preserve);
    void setEnablePlaceholders(bool enable);
    
    // 状态查询
    bool isDebugMode() const;
    bool isPreserveComments() const;
    bool isEnablePlaceholders() const;
    
    // 片段管理
    std::vector<CodeFragment> getFragments() const;
    std::vector<CodeFragment> getFragmentsByType(CodeType type) const;
    CodeFragment getFragment(size_t index) const;
    size_t getFragmentCount() const;
    
    // 占位符管理
    std::unordered_map<std::string, CodeFragment> getPlaceholders() const;
    std::string restoreFromPlaceholders(const std::string& content) const;
    
    // 工具方法
    void clear();
    void reset();
    
    // 调试方法
    void printFragments() const;
    void printPlaceholders() const;
    void printStatistics() const;
};

} // namespace CHTL

#endif // UNIFIED_SCANNER_H