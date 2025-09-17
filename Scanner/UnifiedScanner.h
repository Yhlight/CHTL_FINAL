#ifndef UNIFIEDSCANNER_H
#define UNIFIEDSCANNER_H

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * 代码片段类型
 */
enum class CodeType {
    CHTL,           // CHTL代码
    CHTL_JS,        // CHTL JS代码
    CSS,            // CSS代码
    JS,             // JavaScript代码
    UNKNOWN         // 未知类型
};

/**
 * 代码片段
 */
struct CodeFragment {
    CodeType type;
    std::string content;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    
    CodeFragment(CodeType type, const std::string& content, 
                size_t startLine, size_t startColumn,
                size_t endLine, size_t endColumn)
        : type(type), content(content), startLine(startLine), 
          startColumn(startColumn), endLine(endLine), endColumn(endColumn) {
    }
};

/**
 * 统一扫描器
 * 负责分离CHTL、CHTL JS、CSS、JS代码片段
 */
class UnifiedScanner {
public:
    UnifiedScanner();
    ~UnifiedScanner();

    // 扫描方法
    std::vector<CodeFragment> scan(const std::string& source);
    void setSource(const std::string& source);
    
    // 状态管理
    void reset();
    bool hasMoreCode() const;
    
    // 错误处理
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;

private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    std::vector<std::string> errors;
    
    // 扫描方法
    CodeFragment scanNextFragment();
    CodeFragment scanCHTL();
    CodeFragment scanCHTLJS();
    CodeFragment scanCSS();
    CodeFragment scanJS();
    
    // 辅助方法
    void skipWhitespace();
    void skipComment();
    bool isAtEnd() const;
    char currentChar() const;
    char peekChar() const;
    void advance();
    
    // 边界检测
    bool isCHTLBoundary() const;
    bool isCHTLJSBoundary() const;
    bool isCSSBoundary() const;
    bool isJSBoundary() const;
    
    // 占位符处理
    std::string processPlaceholders(const std::string& code, CodeType type);
    std::string replaceJSWithPlaceholder(const std::string& code);
    std::string replaceCHTLWithPlaceholder(const std::string& code);
    
    // 错误报告
    void reportError(const std::string& message);
    void reportError(size_t line, size_t column, const std::string& message);
    
    // 同步恢复
    void synchronize();
};

} // namespace CHTL

#endif // UNIFIEDSCANNER_H