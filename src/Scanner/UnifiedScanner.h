#ifndef UNIFIED_SCANNER_H
#define UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {

// 代码片段类型
enum class CodeFragmentType {
    CHTL,           // CHTL代码
    CHTL_JS,        // CHTL JS代码
    CSS,            // CSS代码
    JS,             // JavaScript代码
    HTML,           // HTML代码
    UNKNOWN         // 未知类型
};

// 代码片段
struct CodeFragment {
    CodeFragmentType type;
    std::string content;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    std::string placeholder;  // 占位符（用于替换）
    
    CodeFragment(CodeFragmentType t, const std::string& c, size_t sl, size_t sc, size_t el, size_t ec)
        : type(t), content(c), startLine(sl), startColumn(sc), endLine(el), endColumn(ec) {}
};

// 扫描结果
struct ScanResult {
    std::vector<CodeFragment> fragments;
    std::unordered_map<std::string, std::string> placeholders;
    std::string processedCode;
    
    void clear() {
        fragments.clear();
        placeholders.clear();
        processedCode.clear();
    }
};

class UnifiedScanner {
public:
    UnifiedScanner();
    
    // 主要接口
    ScanResult scan(const std::string& source);
    ScanResult scanStyleBlock(const std::string& styleContent);
    ScanResult scanScriptBlock(const std::string& scriptContent);
    
    // 配置
    void setPlaceholderPrefix(const std::string& prefix) { placeholderPrefix_ = prefix; }
    void setEnablePlaceholderMechanism(bool enable) { enablePlaceholderMechanism_ = enable; }
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler);
    
private:
    std::string placeholderPrefix_;
    bool enablePlaceholderMechanism_;
    std::function<void(const std::string&, size_t, size_t)> errorHandler_;
    
    // 扫描状态
    size_t currentPos_;
    size_t currentLine_;
    size_t currentColumn_;
    std::string source_;
    
    // 占位符管理
    std::unordered_map<std::string, std::string> placeholders_;
    size_t placeholderCounter_;
    
    // 字符处理
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    bool isWhitespace(char c) const;
    
    // 边界检测
    bool isCHTLBoundary() const;
    bool isCHTLJSBoundary() const;
    bool isCSSBoundary() const;
    bool isJSBoundary() const;
    bool isHTMLBoundary() const;
    
    // 块检测
    bool isStyleBlock() const;
    bool isScriptBlock() const;
    bool isTemplateBlock() const;
    bool isCustomBlock() const;
    bool isOriginBlock() const;
    bool isImportBlock() const;
    bool isConfigurationBlock() const;
    bool isNamespaceBlock() const;
    
    // 扫描方法
    CodeFragment scanCHTLFragment();
    CodeFragment scanCHTLJSFragment();
    CodeFragment scanCSSFragment();
    CodeFragment scanJSFragment();
    CodeFragment scanHTMLFragment();
    
    // 占位符机制
    std::string createPlaceholder(const std::string& content);
    std::string restorePlaceholders(const std::string& processedCode);
    void registerPlaceholder(const std::string& placeholder, const std::string& content);
    
    // 双指针扫描
    struct ScanWindow {
        size_t start;
        size_t end;
        CodeFragmentType type;
        bool found;
    };
    
    ScanWindow scanWithTwoPointers(const std::string& content, CodeFragmentType targetType);
    
    // 前置截取
    std::string preprocessContent(const std::string& content, CodeFragmentType targetType);
    
    // 语法边界识别
    bool isSyntaxBoundary(const std::string& content, size_t pos) const;
    bool isFunctionBoundary(const std::string& content, size_t pos) const;
    bool isObjectBoundary(const std::string& content, size_t pos) const;
    bool isArrayBoundary(const std::string& content, size_t pos) const;
    bool isConditionalBoundary(const std::string& content, size_t pos) const;
    bool isLoopBoundary(const std::string& content, size_t pos) const;
    
    // 宽判和严判
    bool wideJudge(const std::string& content, size_t pos) const;
    bool strictJudge(const std::string& content, size_t pos) const;
    
    // 错误报告
    void reportError(const std::string& message);
    
    // 辅助方法
    std::string extractBlock(const std::string& content, size_t start, size_t& end) const;
    bool isBlockStart(const std::string& content, size_t pos) const;
    bool isBlockEnd(const std::string& content, size_t pos) const;
    size_t findMatchingBrace(const std::string& content, size_t start) const;
    size_t findMatchingParen(const std::string& content, size_t start) const;
    size_t findMatchingBracket(const std::string& content, size_t start) const;
};

} // namespace CHTL

#endif // UNIFIED_SCANNER_H