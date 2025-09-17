#ifndef UNIFIEDSCANNER_H
#define UNIFIEDSCANNER_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

/**
 * 统一扫描器
 * 负责分离CHTL、CHTL JS、CSS、JS代码片段
 */
class UnifiedScanner {
public:
    enum class CodeType {
        CHTL,
        CHTL_JS,
        CSS,
        JS,
        UNKNOWN
    };
    
    struct CodeFragment {
        CodeType type;
        std::string content;
        size_t start_pos;
        size_t end_pos;
        int line;
        int column;
    };
    
    UnifiedScanner();
    ~UnifiedScanner();
    
    // 主要方法
    std::vector<CodeFragment> scan(const std::string& source);
    std::vector<CodeFragment> scanCHTL(const std::string& source);
    std::vector<CodeFragment> scanCHTLJS(const std::string& source);
    std::vector<CodeFragment> scanCSS(const std::string& source);
    std::vector<CodeFragment> scanJS(const std::string& source);
    
    // 配置方法
    void setDebugMode(bool debug);
    bool isDebugMode() const;
    
    // 占位符管理
    void addPlaceholder(const std::string& placeholder, const std::string& content);
    std::string getPlaceholder(const std::string& placeholder) const;
    void clearPlaceholders();
    
    // 错误处理
    void addError(const std::string& error);
    const std::vector<std::string>& getErrors() const;
    bool hasErrors() const;
    void clearErrors();

private:
    bool debug_mode_;
    std::map<std::string, std::string> placeholders_;
    std::vector<std::string> errors_;
    
    // 内部扫描方法
    CodeFragment scanCHTLBlock(const std::string& source, size_t start_pos);
    CodeFragment scanCHTLJSBlock(const std::string& source, size_t start_pos);
    CodeFragment scanCSSBlock(const std::string& source, size_t start_pos);
    CodeFragment scanJSBlock(const std::string& source, size_t start_pos);
    
    // 边界检测
    bool isCHTLBoundary(const std::string& source, size_t pos);
    bool isCHTLJSBoundary(const std::string& source, size_t pos);
    bool isCSSBoundary(const std::string& source, size_t pos);
    bool isJSBoundary(const std::string& source, size_t pos);
    
    // 占位符处理
    std::string replaceWithPlaceholder(const std::string& content, CodeType type);
    std::string restoreFromPlaceholder(const std::string& content);
    
    // 辅助方法
    size_t findMatchingBrace(const std::string& source, size_t start_pos);
    size_t findMatchingParen(const std::string& source, size_t start_pos);
    size_t findMatchingBracket(const std::string& source, size_t start_pos);
    bool isWhitespace(char c);
    bool isNewline(char c);
    int getLineNumber(const std::string& source, size_t pos);
    int getColumnNumber(const std::string& source, size_t pos);
    
    // 语法边界检测
    bool isCHTLKeyword(const std::string& source, size_t pos);
    bool isCHTLJSKeyword(const std::string& source, size_t pos);
    bool isCSSKeyword(const std::string& source, size_t pos);
    bool isJSKeyword(const std::string& source, size_t pos);
    
    // 特殊语法处理
    bool isSelectorSyntax(const std::string& source, size_t pos);
    bool isResponsiveValueSyntax(const std::string& source, size_t pos);
    bool isTemplateSyntax(const std::string& source, size_t pos);
    bool isCustomSyntax(const std::string& source, size_t pos);
    bool isOriginSyntax(const std::string& source, size_t pos);
    bool isImportSyntax(const std::string& source, size_t pos);
    bool isNamespaceSyntax(const std::string& source, size_t pos);
    bool isConfigurationSyntax(const std::string& source, size_t pos);
};

} // namespace CHTL

#endif // UNIFIEDSCANNER_H