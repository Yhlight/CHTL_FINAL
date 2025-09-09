#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class CodeType {
    CHTL,       // CHTL代码
    CHTL_JS,    // CHTL JS代码
    CSS,        // CSS代码
    JS,         // JavaScript代码
    HTML,       // HTML代码
    UNKNOWN     // 未知类型
};

struct CodeFragment {
    CodeType type;
    std::string content;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    std::string originalContent; // 原始内容（包含占位符）
};

class UnifiedScanner {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    
    // 状态管理
    bool inCHTLBlock;
    bool inCHTLJSBlock;
    bool inCSSBlock;
    bool inJSBlock;
    bool inHTMLBlock;
    
    // 占位符管理
    std::map<std::string, std::string> placeholders;
    size_t placeholderCounter;
    
    // 扫描状态
    struct ScanState {
        bool inString;
        bool inComment;
        bool inMultiLineComment;
        bool inTemplate;
        bool inCustom;
        bool inOrigin;
        bool inImport;
        bool inNamespace;
        bool inConfiguration;
        char stringDelimiter;
        int braceLevel;
        int bracketLevel;
        int parenLevel;
    } state;
    
    // 辅助方法
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipNewline();
    
    // 状态检查
    bool isCHTLStart() const;
    bool isCHTLJSEnd() const;
    bool isCSSStart() const;
    bool isJSEnd() const;
    bool isHTMLStart() const;
    bool isHTMLEnd() const;
    
    // 块检测
    bool isTemplateBlock() const;
    bool isCustomBlock() const;
    bool isOriginBlock() const;
    bool isImportBlock() const;
    bool isNamespaceBlock() const;
    bool isConfigurationBlock() const;
    bool isStyleBlock() const;
    bool isScriptBlock() const;
    
    // 扫描方法
    CodeFragment scanCHTLBlock();
    CodeFragment scanCHTLJSBlock();
    CodeFragment scanCSSBlock();
    CodeFragment scanJSBlock();
    CodeFragment scanHTMLBlock();
    CodeFragment scanUnknownBlock();
    
    // 占位符处理
    std::string createPlaceholder(const std::string& content);
    std::string restorePlaceholders(const std::string& content);
    void registerPlaceholder(const std::string& placeholder, const std::string& content);
    
    // 边界检测
    bool isCodeBoundary() const;
    bool isNestedBlock() const;
    bool isStringLiteral() const;
    bool isComment() const;
    bool isMultiLineComment() const;
    
    // 智能扩增
    void expandBoundary();
    void contractBoundary();
    bool isSafeBoundary() const;
    
    // 宽判严判
    bool isWideJudge() const;
    bool isStrictJudge() const;
    
    // 错误处理
    void throwError(const std::string& message) const;
    void synchronize();
    
public:
    UnifiedScanner();
    ~UnifiedScanner() = default;
    
    // 主要扫描方法
    std::vector<CodeFragment> scan(const std::string& source);
    std::vector<std::string> scanAsStrings(const std::string& source);
    
    // 状态查询
    bool isAtEnd() const;
    size_t getCurrentPosition() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    
    // 占位符管理
    void clearPlaceholders();
    std::map<std::string, std::string> getPlaceholders() const;
    
    // 调试方法
    void enableDebugMode(bool enable = true);
    std::string getDebugInfo() const;
    
    // 统计信息
    struct ScanStats {
        size_t totalFragments;
        size_t chtlFragments;
        size_t chtlJSFragments;
        size_t cssFragments;
        size_t jsFragments;
        size_t htmlFragments;
        size_t unknownFragments;
        double scanTime;
    };
    
    ScanStats getStats() const;
    void resetStats();
    
private:
    bool debugMode;
    ScanStats stats;
};

} // namespace CHTL