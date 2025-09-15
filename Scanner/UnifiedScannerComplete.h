#ifndef UNIFIED_SCANNER_COMPLETE_H
#define UNIFIED_SCANNER_COMPLETE_H

#include "UnifiedScanner.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <functional>
#include <regex>

namespace CHTL {

/**
 * 完整的统一扫描器
 * 负责精准分离CHTL、CHTL JS、CSS、JS代码片段
 * 严格遵循CHTL.md中定义的语法规范
 * 使用占位符机制确保语法边界不被破坏
 */
class UnifiedScannerComplete : public UnifiedScanner {
public:
    // 扩展的代码类型
    enum class ExtendedCodeType {
        CHTL_ELEMENT,       // CHTL元素
        CHTL_TEMPLATE,      // CHTL模板
        CHTL_CUSTOM,        // CHTL自定义
        CHTL_NAMESPACE,     // CHTL命名空间
        CHTL_CONFIG,        // CHTL配置
        CHTL_IMPORT,        // CHTL导入
        CHTL_ORIGIN,        // CHTL原始嵌入
        CHTL_STYLE_LOCAL,   // CHTL局部样式
        CHTL_SCRIPT_LOCAL,  // CHTL局部脚本
        CHTL_JS_SPECIAL,    // CHTL JS特殊语法
        CSS_GLOBAL,         // 全局CSS
        JS_GLOBAL,          // 全局JS
        UNKNOWN_EXT
    };
    
    // 扩展的代码片段
    struct ExtendedCodeFragment : public CodeFragment {
        ExtendedCodeType extended_type;
        std::string placeholder_id;
        std::map<std::string, std::string> attributes;
        std::vector<std::shared_ptr<ExtendedCodeFragment>> children;
        std::string context;
        int depth_level;
        bool is_placeholder;
        bool has_syntax_boundary;
        
        ExtendedCodeFragment() : depth_level(0), is_placeholder(false), has_syntax_boundary(false) {}
    };
    
    // 占位符配置
    struct PlaceholderConfig {
        std::string prefix;
        std::string suffix;
        bool preserve_whitespace;
        bool preserve_newlines;
        bool preserve_indentation;
        
        PlaceholderConfig() 
            : prefix("__CHTL_PLACEHOLDER_"), suffix("__"), 
              preserve_whitespace(true), preserve_newlines(true), preserve_indentation(true) {}
    };
    
    // 扫描配置
    struct ScanConfig {
        bool enable_placeholder_mode;
        bool preserve_syntax_boundaries;
        bool enable_context_awareness;
        bool enable_nesting_detection;
        bool enable_error_recovery;
        int max_nesting_depth;
        
        ScanConfig() 
            : enable_placeholder_mode(true), preserve_syntax_boundaries(true),
              enable_context_awareness(true), enable_nesting_detection(true),
              enable_error_recovery(true), max_nesting_depth(10) {}
    };
    
    UnifiedScannerComplete();
    ~UnifiedScannerComplete();
    
    // 重写基类方法
    std::vector<CodeFragment> scan(const std::string& source) override;
    
    // 扩展的扫描方法
    std::vector<ExtendedCodeFragment> scanExtended(const std::string& source);
    std::vector<ExtendedCodeFragment> scanWithPlaceholders(const std::string& source);
    std::vector<ExtendedCodeFragment> scanWithContext(const std::string& source, const std::string& context = "");
    
    // 精准的语法扫描
    std::vector<ExtendedCodeFragment> scanCHTLSyntax(const std::string& source);
    std::vector<ExtendedCodeFragment> scanCHTLJSSyntax(const std::string& source);
    std::vector<ExtendedCodeFragment> scanLocalStyleBlocks(const std::string& source);
    std::vector<ExtendedCodeFragment> scanLocalScriptBlocks(const std::string& source);
    std::vector<ExtendedCodeFragment> scanTemplateSyntax(const std::string& source);
    std::vector<ExtendedCodeFragment> scanCustomSyntax(const std::string& source);
    std::vector<ExtendedCodeFragment> scanSpecialSyntax(const std::string& source);
    
    // 占位符系统
    std::string createPlaceholder(const ExtendedCodeFragment& fragment);
    std::string createPlaceholder(const std::string& content, ExtendedCodeType type);
    bool isPlaceholder(const std::string& text) const;
    ExtendedCodeFragment resolvePlaceholder(const std::string& placeholder) const;
    std::string replacePlaceholders(const std::string& text);
    std::string restorePlaceholders(const std::string& text);
    
    // 语法边界检测
    bool isCHTLElementBoundary(const std::string& source, size_t pos) const;
    bool isCHTLTemplateBoundary(const std::string& source, size_t pos) const;
    bool isCHTLCustomBoundary(const std::string& source, size_t pos) const;
    bool isCHTLNamespaceBoundary(const std::string& source, size_t pos) const;
    bool isCHTLConfigBoundary(const std::string& source, size_t pos) const;
    bool isCHTLImportBoundary(const std::string& source, size_t pos) const;
    bool isCHTLOriginBoundary(const std::string& source, size_t pos) const;
    bool isCHTLStyleBlockBoundary(const std::string& source, size_t pos) const;
    bool isCHTLScriptBlockBoundary(const std::string& source, size_t pos) const;
    bool isCHTLJSSpecialBoundary(const std::string& source, size_t pos) const;
    
    // 高级语法分析
    bool isCHTLVariableSyntax(const std::string& source, size_t pos) const;
    bool isCHTLSpecializationSyntax(const std::string& source, size_t pos) const;
    bool isCHTLStyleGroupSyntax(const std::string& source, size_t pos) const;
    bool isCHTLPropertyReferenceSyntax(const std::string& source, size_t pos) const;
    bool isCHTLResponsiveValueSyntax(const std::string& source, size_t pos) const;
    bool isCHTLOperatorSyntax(const std::string& source, size_t pos) const;
    
    // 内容提取
    ExtendedCodeFragment extractCHTLElement(const std::string& source, size_t start_pos);
    ExtendedCodeFragment extractCHTLTemplate(const std::string& source, size_t start_pos);
    ExtendedCodeFragment extractCHTLCustom(const std::string& source, size_t start_pos);
    ExtendedCodeFragment extractCHTLNamespace(const std::string& source, size_t start_pos);
    ExtendedCodeFragment extractCHTLConfig(const std::string& source, size_t start_pos);
    ExtendedCodeFragment extractCHTLImport(const std::string& source, size_t start_pos);
    ExtendedCodeFragment extractCHTLOrigin(const std::string& source, size_t start_pos);
    ExtendedCodeFragment extractCHTLStyleBlock(const std::string& source, size_t start_pos);
    ExtendedCodeFragment extractCHTLScriptBlock(const std::string& source, size_t start_pos);
    ExtendedCodeFragment extractCHTLJSSpecial(const std::string& source, size_t start_pos);
    
    // 属性解析
    std::map<std::string, std::string> parseAttributes(const std::string& attr_string);
    std::vector<std::string> parseStyleGroups(const std::string& style_string);
    std::map<std::string, std::string> parseStyleProperties(const std::string& style_string);
    std::vector<std::string> parseInheritanceList(const std::string& inherit_string);
    
    // 嵌套处理
    std::vector<ExtendedCodeFragment> extractNestedFragments(const ExtendedCodeFragment& parent);
    int calculateNestingDepth(const std::string& source, size_t pos) const;
    bool validateNestingRules(const ExtendedCodeFragment& fragment) const;
    
    // 配置管理
    void setPlaceholderConfig(const PlaceholderConfig& config);
    PlaceholderConfig getPlaceholderConfig() const;
    void setScanConfig(const ScanConfig& config);
    ScanConfig getScanConfig() const;
    
    // 错误处理和恢复
    void setErrorRecoveryMode(bool enable);
    bool getErrorRecoveryMode() const;
    std::vector<std::string> getSyntaxErrors() const;
    void clearSyntaxErrors();
    bool recoverFromSyntaxError(const std::string& source, size_t& pos);
    
    // 统计信息
    size_t getFragmentCount() const;
    size_t getPlaceholderCount() const;
    std::map<ExtendedCodeType, size_t> getFragmentStatistics() const;
    std::string getScannerStatistics() const;
    
    // 工具方法
    std::string generateUniqueId() const;
    std::string normalizeWhitespace(const std::string& text) const;
    std::string preserveIndentation(const std::string& text, const std::string& source, size_t pos) const;
    
    // 调试和验证
    void enableVerboseMode(bool enable);
    bool isVerboseMode() const;
    void dumpFragments(const std::vector<ExtendedCodeFragment>& fragments) const;
    bool validateFragmentIntegrity(const std::vector<ExtendedCodeFragment>& fragments) const;

private:
    PlaceholderConfig placeholder_config_;
    ScanConfig scan_config_;
    bool error_recovery_mode_;
    bool verbose_mode_;
    
    // 内部状态
    mutable size_t next_placeholder_id_;
    std::map<std::string, ExtendedCodeFragment> placeholder_map_;
    std::vector<std::string> syntax_errors_;
    std::map<ExtendedCodeType, size_t> fragment_statistics_;
    
    // 正则表达式缓存
    mutable std::map<std::string, std::regex> regex_cache_;
    
    // 内部辅助方法
    std::regex getRegex(const std::string& pattern) const;
    size_t findMatchingDelimiter(const std::string& source, size_t start_pos, 
                                 char open_char, char close_char) const;
    size_t findNextSyntaxBoundary(const std::string& source, size_t start_pos) const;
    size_t skipWhitespaceAndComments(const std::string& source, size_t pos) const;
    size_t skipString(const std::string& source, size_t pos, char quote_char) const;
    
    // 特殊语法处理
    bool matchKeyword(const std::string& source, size_t pos, const std::string& keyword) const;
    bool matchPattern(const std::string& source, size_t pos, const std::string& pattern) const;
    std::string extractUntilDelimiter(const std::string& source, size_t start_pos, 
                                     const std::string& delimiter) const;
    std::string extractBetweenDelimiters(const std::string& source, size_t start_pos,
                                        const std::string& start_delim, const std::string& end_delim) const;
    
    // 上下文分析
    std::string analyzeContext(const std::string& source, size_t pos) const;
    bool isInStringLiteral(const std::string& source, size_t pos) const;
    bool isInComment(const std::string& source, size_t pos) const;
    bool isInCHTLBlock(const std::string& source, size_t pos) const;
    bool isInCHTLJSBlock(const std::string& source, size_t pos) const;
    
    // 错误处理
    void addSyntaxError(const std::string& error, int line = 0, int column = 0);
    void logVerbose(const std::string& message) const;
    
    // 初始化
    void initializeRegexCache();
    void initializeDefaultConfig();
};

} // namespace CHTL

#endif // UNIFIED_SCANNER_COMPLETE_H