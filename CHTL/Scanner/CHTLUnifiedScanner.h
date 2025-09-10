#ifndef CHTL_UNIFIED_SCANNER_H
#define CHTL_UNIFIED_SCANNER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Common/CommonTypes.h"

namespace CHTL {

class CHTLUnifiedScanner {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    
    // 配置
    bool debugMode;
    bool strictMode;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 占位符机制
    std::map<std::string, std::string> placeholders;
    size_t placeholderCounter;
    
    // 辅助方法
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void advance(size_t count);
    void skipWhitespace();
    void skipLine();
    
    // 位置信息
    void updatePosition();
    size_t getCurrentLine() const { return line; }
    size_t getCurrentColumn() const { return column; }
    
    // 错误处理
    void addError(const std::string& message);
    void addError(const std::string& message, size_t line, size_t column);
    void addWarning(const std::string& message);
    void addWarning(const std::string& message, size_t line, size_t column);
    
    // 扫描方法
    ScanResult scanSource();
    CodeFragment scanCHTLFragment();
    CodeFragment scanCHTLJSFragment();
    CodeFragment scanCSSFragment();
    CodeFragment scanJSFragment();
    
    // 识别代码类型
    std::string identifyCodeType();
    bool isCHTLSyntax();
    bool isCHTLJSSyntax();
    bool isCSSSyntax();
    bool isJSSyntax();
    
    // 占位符处理
    std::string createPlaceholder(const std::string& content);
    std::string restorePlaceholder(const std::string& placeholder);
    void processPlaceholders();
    
    // 代码分离
    std::string separateCHTLAndJS(const std::string& code);
    std::string separateCSSAndJS(const std::string& code);
    
    // 语法边界检测
    bool isCHTLBoundary();
    bool isCHTLJSBoundary();
    bool isCSSBoundary();
    bool isJSBoundary();
    
    // 块检测
    bool isBlockStart();
    bool isBlockEnd();
    bool isStyleBlock();
    bool isScriptBlock();
    bool isTemplateBlock();
    bool isCustomBlock();
    bool isOriginBlock();
    bool isImportBlock();
    bool isNamespaceBlock();
    bool isConfigurationBlock();
    
    // 字符串和注释处理
    std::string scanString();
    std::string scanComment();
    void skipString();
    void skipComment();
    
    // 宽判严判机制
    bool isWideScope();
    bool isStrictScope();
    
    // 可变长度切片
    std::string sliceCode(size_t start, size_t end);
    void adjustSliceBoundary(size_t& start, size_t& end);
    
    // 智能扩增
    void expandSlice(size_t& start, size_t& end);
    void shrinkSlice(size_t& start, size_t& end);

public:
    CHTLUnifiedScanner();
    explicit CHTLUnifiedScanner(const std::string& source);
    
    // 设置源代码
    void setSource(const std::string& source);
    
    // 扫描
    ScanResult scan(const std::string& inputFile);
    ScanResult scan();
    
    // 配置
    void setDebugMode(bool debug) { debugMode = debug; }
    void setStrictMode(bool strict) { strictMode = strict; }
    
    // 获取错误和警告
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    
    // 清除错误和警告
    void clearMessages();
    
    // 重置扫描器
    void reset();
    
    // 验证扫描结果
    bool validate(const ScanResult& result) const;
    
    // 调试信息
    std::string getDebugInfo() const;
};

} // namespace CHTL

#endif // CHTL_UNIFIED_SCANNER_H