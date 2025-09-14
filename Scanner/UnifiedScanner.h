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
    bool isPlaceholder;      // 是否为占位符
};

class UnifiedScanner {
public:
    UnifiedScanner();
    ~UnifiedScanner() = default;

    // 主要扫描函数 - 分离CHTL、CHTL JS、CSS、JS代码
    std::vector<CodeFragment> scan(const std::string& sourceCode);
    
    // 占位符机制 - 用于分离JS和CHTL JS代码
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
    // 核心扫描算法 - 宽判严判机制
    std::vector<CodeFragment> performScan(const std::string& sourceCode);
    
    // 宽判 - 处理大块CHTL代码
    std::vector<CodeFragment> wideScan(const std::string& content);
    
    // 严判 - 处理CHTL JS和JS混合代码
    std::vector<CodeFragment> strictScan(const std::string& content);
    
    // 占位符机制 - 分离JS和CHTL JS
    std::string applyPlaceholderMechanism(const std::string& content);
    
    // 语法边界识别
    bool detectCHTLSyntaxBoundary(const std::string& content, size_t pos);
    bool detectCHTLJSSyntaxBoundary(const std::string& content, size_t pos);
    
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
    size_t findBlockEnd(const std::string& content, size_t start);
    
    // 调试
    void debugFragment(const CodeFragment& fragment) const;
};

} // namespace CHTL