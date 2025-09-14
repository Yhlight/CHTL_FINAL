#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

enum class CodeType {
    CHTL,
    CHTL_JS,
    CSS,
    JS
};

struct CodeFragment {
    CodeType type;
    std::string content;
    size_t startPos;
    size_t endPos;
    std::string placeholder; // 用于占位符机制
};

class UnifiedScanner {
public:
    UnifiedScanner();
    ~UnifiedScanner() = default;

    // 主要扫描函数
    std::vector<CodeFragment> scan(const std::string& sourceCode);
    
    // 占位符机制
    std::string createPlaceholder(const std::string& type = "JS_CODE");
    std::string decodePlaceholder(const std::string& content);
    
    // 语法边界检测
    bool isCHTLSyntax(const std::string& content);
    bool isCHTLJSSyntax(const std::string& content);
    bool isCSSSyntax(const std::string& content);
    bool isJSSyntax(const std::string& content);
    
    // 局部样式块和脚本块处理
    std::vector<CodeFragment> extractLocalStyleBlocks(const std::string& content);
    std::vector<CodeFragment> extractLocalScriptBlocks(const std::string& content);
    
    // 配置
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    void setPlaceholderPrefix(const std::string& prefix) { placeholderPrefix_ = prefix; }

private:
    // 核心扫描算法
    std::vector<CodeFragment> performScan(const std::string& sourceCode);
    
    // 双指针扫描
    std::vector<CodeFragment> dualPointerScan(const std::string& content);
    
    // 前置截取
    std::string preExtract(const std::string& content, size_t& pos);
    
    // 语法边界识别
    bool detectSyntaxBoundary(const std::string& content, size_t pos);
    
    // 占位符管理
    std::map<std::string, std::string> placeholderMap_;
    std::string placeholderPrefix_;
    int placeholderCounter_;
    
    // 配置
    bool debugMode_;
    
    // 辅助函数
    std::string trim(const std::string& str);
    bool isWhitespace(char c);
    bool isCommentStart(const std::string& content, size_t pos);
    bool isCommentEnd(const std::string& content, size_t pos, bool multiLine);
    bool isStringDelimiter(char c);
    bool isBlockStart(const std::string& content, size_t pos);
    bool isBlockEnd(const std::string& content, size_t pos);
};

} // namespace CHTL