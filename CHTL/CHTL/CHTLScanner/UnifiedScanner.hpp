#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <CHTL/CHTLLexer/Token.hpp>
#include <CHTL/CHTLLexer/Lexer.hpp>

namespace CHTL {

class UnifiedScanner {
public:
    // 占位符类型
    enum class PlaceholderType {
        CHTL,           // CHTL代码占位符
        HTML,           // HTML代码占位符
        CSS,            // CSS代码占位符
        JAVASCRIPT,     // JavaScript代码占位符
        CJJS,           // CHTL JS代码占位符
        CUSTOM          // 自定义类型占位符
    };
    
    // 占位符信息
    struct PlaceholderInfo {
        std::string name;           // 占位符名称
        PlaceholderType type;       // 占位符类型
        std::string content;        // 占位符内容
        size_t startPos;           // 开始位置
        size_t endPos;             // 结束位置
        std::map<std::string, std::string> attributes; // 占位符属性
    };
    
    // 代码块信息
    struct CodeBlock {
        std::string content;        // 代码内容
        PlaceholderType type;       // 代码类型
        size_t startPos;           // 开始位置
        size_t endPos;             // 结束位置
        std::vector<PlaceholderInfo> placeholders; // 包含的占位符
    };
    
    // 扫描结果
    struct ScanResult {
        std::vector<CodeBlock> codeBlocks;      // 代码块列表
        std::vector<PlaceholderInfo> placeholders; // 占位符列表
        std::map<std::string, std::string> metadata; // 元数据
        bool success;                           // 扫描是否成功
        std::string errorMessage;              // 错误信息
    };
    
    // 构造函数
    UnifiedScanner();
    explicit UnifiedScanner(const std::string& input);
    
    // 设置输入
    void setInput(const std::string& input);
    
    // 扫描方法
    ScanResult scan();
    ScanResult scanWithPlaceholders();
    ScanResult scanWithCodeSeparation();
    
    // 占位符处理
    void registerPlaceholder(const std::string& name, PlaceholderType type, 
                           const std::string& content);
    void unregisterPlaceholder(const std::string& name);
    std::vector<PlaceholderInfo> findPlaceholders(const std::string& content) const;
    std::string replacePlaceholders(const std::string& content) const;
    
    // 代码分离
    std::vector<CodeBlock> separateCodeBlocks(const std::string& content) const;
    std::string mergeCodeBlocks(const std::vector<CodeBlock>& blocks) const;
    
    // 智能分析
    PlaceholderType detectCodeType(const std::string& content) const;
    std::vector<std::string> extractKeywords(const std::string& content) const;
    std::map<std::string, std::string> extractMetadata(const std::string& content) const;
    
    // 验证和错误处理
    bool validatePlaceholders(const std::vector<PlaceholderInfo>& placeholders) const;
    std::vector<std::string> getValidationErrors() const;
    
    // 配置选项
    void setPlaceholderPrefix(const std::string& prefix) { placeholderPrefix_ = prefix; }
    void setPlaceholderSuffix(const std::string& suffix) { placeholderSuffix_ = suffix; }
    void setEnableSmartDetection(bool enable) { enableSmartDetection_ = enable; }
    void setEnableCodeSeparation(bool enable) { enableCodeSeparation_ = enable; }
    void setEnablePlaceholderReplacement(bool enable) { enablePlaceholderReplacement_ = enable; }
    
    // 获取配置
    std::string getPlaceholderPrefix() const { return placeholderPrefix_; }
    std::string getPlaceholderSuffix() const { return placeholderSuffix_; }
    bool isSmartDetectionEnabled() const { return enableSmartDetection_; }
    bool isCodeSeparationEnabled() const { return enableCodeSeparation_; }
    bool isPlaceholderReplacementEnabled() const { return enablePlaceholderReplacement_; }
    
    // 统计信息
    size_t getPlaceholderCount() const { return placeholders_.size(); }
    size_t getCodeBlockCount() const { return codeBlocks_.size(); }
    std::map<PlaceholderType, size_t> getPlaceholderTypeCounts() const;
    std::map<PlaceholderType, size_t> getCodeBlockTypeCounts() const;
    
    // 清理
    void clear();
    void reset();
    
private:
    std::string input_;                    // 输入内容
    std::map<std::string, PlaceholderInfo> placeholders_; // 占位符映射
    std::vector<CodeBlock> codeBlocks_;    // 代码块列表
    std::vector<std::string> validationErrors_; // 验证错误
    
    // 配置选项
    std::string placeholderPrefix_;        // 占位符前缀
    std::string placeholderSuffix_;        // 占位符后缀
    bool enableSmartDetection_;           // 启用智能检测
    bool enableCodeSeparation_;           // 启用代码分离
    bool enablePlaceholderReplacement_;   // 启用占位符替换
    
    // 内部方法
    void initializeDefaults();
    void parsePlaceholders(const std::string& content);
    void parseCodeBlocks(const std::string& content);
    PlaceholderType detectTypeFromContent(const std::string& content) const;
    PlaceholderType detectTypeFromKeywords(const std::vector<std::string>& keywords) const;
    std::vector<std::string> tokenizeContent(const std::string& content) const;
    bool isValidPlaceholderName(const std::string& name) const;
    bool isValidPlaceholderType(PlaceholderType type) const;
    std::string formatPlaceholder(const PlaceholderInfo& placeholder) const;
    std::string formatCodeBlock(const CodeBlock& block) const;
    
    // 错误处理
    void addValidationError(const std::string& error);
    void clearValidationErrors();
    
    // 辅助方法
    std::string trim(const std::string& str) const;
    std::vector<std::string> split(const std::string& str, char delimiter) const;
    bool startsWith(const std::string& str, const std::string& prefix) const;
    bool endsWith(const std::string& str, const std::string& suffix) const;
    std::string toLowerCase(const std::string& str) const;
    std::string toUpperCase(const std::string& str) const;
    
    // 正则表达式辅助
    std::vector<std::string> extractMatches(const std::string& content, 
                                          const std::string& pattern) const;
    bool matchesPattern(const std::string& content, 
                       const std::string& pattern) const;
    
    // 位置计算
    size_t calculateLineNumber(const std::string& content, size_t position) const;
    size_t calculateColumnNumber(const std::string& content, size_t position) const;
    std::pair<size_t, size_t> calculatePosition(const std::string& content, 
                                               size_t position) const;
};

} // namespace CHTL