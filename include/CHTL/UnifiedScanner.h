#pragma once

#include "CHTL/Common.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

/**
 * 统一扫描器 - CHTL项目最核心的组件
 * 负责分离CHTL、CHTL JS、CSS、JS代码片段
 */
class UnifiedScanner {
public:
    // 代码片段类型
    enum class FragmentType {
        CHTL,           // CHTL语法片段
        CHTL_JS,        // CHTL JS语法片段
        CSS,            // CSS代码片段
        JS,             // JavaScript代码片段
        MIXED           // 混合代码片段
    };

    // 代码片段结构
    struct CodeFragment {
        FragmentType type;
        std::string content;
        SourceLocation location;
        std::map<std::string, std::string> placeholders; // 占位符映射
        
        CodeFragment(FragmentType t, const std::string& c, const SourceLocation& loc)
            : type(t), content(c), location(loc) {}
    };

    // 扫描结果
    struct ScanResult {
        std::vector<CodeFragment> fragments;
        std::map<std::string, std::string> globalPlaceholders;
        bool success;
        std::string errorMessage;
        
        ScanResult() : success(false) {}
    };

public:
    UnifiedScanner();
    ~UnifiedScanner();

    /**
     * 扫描源代码，分离不同语言的代码片段
     * @param sourceCode 源代码
     * @return 扫描结果
     */
    ScanResult scan(const std::string& sourceCode);

    /**
     * 设置占位符前缀
     * @param prefix 占位符前缀
     */
    void setPlaceholderPrefix(const std::string& prefix);

    /**
     * 启用/禁用调试模式
     * @param enabled 是否启用
     */
    void setDebugMode(bool enabled);

private:
    // 内部扫描方法
    ScanResult scanGlobalStyle(const std::string& content, SourceLocation start);
    ScanResult scanScript(const std::string& content, SourceLocation start, bool isLocal);
    ScanResult scanMixedCode(const std::string& content, SourceLocation start);
    
    // 占位符管理
    std::string createPlaceholder(const std::string& type, size_t index);
    std::string restorePlaceholders(const std::string& content, 
                                   const std::map<std::string, std::string>& placeholders);
    
    // 语法边界识别
    bool isCHTLSyntax(const std::string& content, size_t pos);
    bool isCHTLJSSyntax(const std::string& content, size_t pos);
    bool isJSSyntax(const std::string& content, size_t pos);
    bool isCSSSyntax(const std::string& content, size_t pos);
    
    // 双指针扫描算法
    std::pair<size_t, size_t> findSyntaxBoundary(const std::string& content, 
                                                size_t start, 
                                                FragmentType targetType);
    
    // 前置截取算法
    std::string preprocessCode(const std::string& content, FragmentType type);
    
    // 宽判和严判
    bool isWideJudge(const std::string& content, size_t pos);
    bool isStrictJudge(const std::string& content, size_t pos);
    
    // 类型判断
    FragmentType determineFragmentType(const std::string& content);
    
    // 边界查找方法
    size_t findCHTLBoundary(const std::string& content, size_t start);
    size_t findCHTLJSBoundary(const std::string& content, size_t start);
    size_t findJSBoundary(const std::string& content, size_t start);
    size_t findCSSBoundary(const std::string& content, size_t start);
    size_t findMixedBoundary(const std::string& content, size_t start);
    
    // 处理方法
    std::string processAttributeOperations(const std::string& content);
    std::string processConditionalExpressions(const std::string& content);
    std::string processTemplateVariables(const std::string& content);
    std::string processCustomVariables(const std::string& content);
    std::string processStyleGroups(const std::string& content);
    std::string processCHTLJSSyntax(const std::string& content, 
                                   std::map<std::string, std::string>& placeholders);
    std::string processCHTLSyntax(const std::string& content, 
                                 std::map<std::string, std::string>& placeholders);
    std::string processJSSyntax(const std::string& content, 
                               std::map<std::string, std::string>& placeholders);
    std::string removeComments(const std::string& content);
    std::string processStrings(const std::string& content);
    
    // 初始化方法
    void initializePatterns();

private:
    std::string placeholderPrefix_;
    bool debugMode_;
    size_t placeholderCounter_;
    std::map<std::string, std::string> placeholderMap_;
    
    // 语法边界模式
    std::vector<std::string> chtlPatterns_;
    std::vector<std::string> chtlJSPatterns_;
    std::vector<std::string> jsPatterns_;
    std::vector<std::string> cssPatterns_;
};

} // namespace CHTL