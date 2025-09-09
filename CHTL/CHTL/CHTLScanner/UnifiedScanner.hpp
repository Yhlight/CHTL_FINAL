#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief 代码类型枚举
 */
enum class CodeType {
    CHTL,       // CHTL 代码
    CHTL_JS,    // CHTL JS 代码
    CSS,        // CSS 代码
    JAVASCRIPT  // JavaScript 代码
};

/**
 * @brief 代码块信息
 */
struct CodeBlock {
    CodeType type;
    std::string content;
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    std::string language;
    
    CodeBlock() : type(CodeType::CHTL), startLine(0), startColumn(0), endLine(0), endColumn(0) {}
};

/**
 * @brief 统一扫描器
 * 
 * 根据 CHTL.md 文档实现
 * 支持 CHTL、CHTL JS、CSS、JS 代码的智能分离
 */
class UnifiedScanner {
public:
    UnifiedScanner();
    ~UnifiedScanner() = default;
    
    // 扫描和分离代码
    std::vector<CodeBlock> scan(const std::string& content);
    std::vector<CodeBlock> scanFromFile(const std::string& filePath);
    
    // 代码类型检测
    CodeType detectCodeType(const std::string& content) const;
    CodeType detectCodeTypeByExtension(const std::string& filePath) const;
    CodeType detectCodeTypeByContent(const std::string& content) const;
    
    // 代码块提取
    std::vector<CodeBlock> extractCHTLBlocks(const std::string& content) const;
    std::vector<CodeBlock> extractCHTLJSBlocks(const std::string& content) const;
    std::vector<CodeBlock> extractCSSBlocks(const std::string& content) const;
    std::vector<CodeBlock> extractJavaScriptBlocks(const std::string& content) const;
    
    // 代码块验证
    bool validateCodeBlock(const CodeBlock& block) const;
    bool validateCHTLBlock(const CodeBlock& block) const;
    bool validateCHTLJSBlock(const CodeBlock& block) const;
    bool validateCSSBlock(const CodeBlock& block) const;
    bool validateJavaScriptBlock(const CodeBlock& block) const;
    
    // 代码块转换
    std::string convertToCHTL(const CodeBlock& block) const;
    std::string convertToCHTLJS(const CodeBlock& block) const;
    std::string convertToCSS(const CodeBlock& block) const;
    std::string convertToJavaScript(const CodeBlock& block) const;
    
    // 代码块合并
    std::string mergeBlocks(const std::vector<CodeBlock>& blocks, CodeType targetType) const;
    std::string mergeToCHTL(const std::vector<CodeBlock>& blocks) const;
    std::string mergeToCHTLJS(const std::vector<CodeBlock>& blocks) const;
    std::string mergeToCSS(const std::vector<CodeBlock>& blocks) const;
    std::string mergeToJavaScript(const std::vector<CodeBlock>& blocks) const;
    
    // 代码块过滤
    std::vector<CodeBlock> filterByType(const std::vector<CodeBlock>& blocks, CodeType type) const;
    std::vector<CodeBlock> filterByLanguage(const std::vector<CodeBlock>& blocks, const std::string& language) const;
    
    // 代码块统计
    size_t countBlocks(const std::vector<CodeBlock>& blocks, CodeType type) const;
    size_t countLines(const std::vector<CodeBlock>& blocks, CodeType type) const;
    size_t countCharacters(const std::vector<CodeBlock>& blocks, CodeType type) const;
    
    // 代码块格式化
    std::string formatCodeBlock(const CodeBlock& block) const;
    std::string formatBlocks(const std::vector<CodeBlock>& blocks) const;
    
private:
    // 内部方法
    bool isCHTLKeyword(const std::string& word) const;
    bool isCHTLJSKeyword(const std::string& word) const;
    bool isCSSKeyword(const std::string& word) const;
    bool isJavaScriptKeyword(const std::string& word) const;
    
    bool isCHTLComment(const std::string& line) const;
    bool isCHTLJSComment(const std::string& line) const;
    bool isCSSComment(const std::string& line) const;
    bool isJavaScriptComment(const std::string& line) const;
    
    bool isCHTLBlockStart(const std::string& line) const;
    bool isCHTLJSBlockStart(const std::string& line) const;
    bool isCSSBlockStart(const std::string& line) const;
    bool isJavaScriptBlockStart(const std::string& line) const;
    
    bool isCHTLBlockEnd(const std::string& line) const;
    bool isCHTLJSBlockEnd(const std::string& line) const;
    bool isCSSBlockEnd(const std::string& line) const;
    bool isJavaScriptBlockEnd(const std::string& line) const;
    
    std::string extractLanguageFromBlock(const std::string& content) const;
    std::string normalizeCodeBlock(const CodeBlock& block) const;
    
    // 关键词列表
    std::vector<std::string> chtlKeywords_;
    std::vector<std::string> chtljsKeywords_;
    std::vector<std::string> cssKeywords_;
    std::vector<std::string> javascriptKeywords_;
    
    // 块开始/结束标记
    std::vector<std::string> chtlBlockStarters_;
    std::vector<std::string> chtljsBlockStarters_;
    std::vector<std::string> cssBlockStarters_;
    std::vector<std::string> javascriptBlockStarters_;
    
    std::vector<std::string> chtlBlockEnders_;
    std::vector<std::string> chtljsBlockEnders_;
    std::vector<std::string> cssBlockEnders_;
    std::vector<std::string> javascriptBlockEnders_;
};

} // namespace CHTL