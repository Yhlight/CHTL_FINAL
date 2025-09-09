#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <set>
#include <regex>

namespace CHTL {

/**
 * @brief 扫描模式枚举
 */
enum class ScanMode {
    BROAD,          // 宽泛模式
    STRICT          // 严格模式
};

/**
 * @brief 占位符类型枚举
 */
enum class PlaceholderType {
    VARIABLE,       // 变量占位符
    FUNCTION,       // 函数占位符
    EXPRESSION,     // 表达式占位符
    BLOCK,          // 代码块占位符
    UNKNOWN         // 未知类型
};

/**
 * @brief 占位符
 */
class Placeholder {
public:
    Placeholder(const std::string& content, PlaceholderType type, int start, int end);
    ~Placeholder() = default;

    std::string getContent() const { return m_content; }
    PlaceholderType getType() const { return m_type; }
    int getStart() const { return m_start; }
    int getEnd() const { return m_end; }
    int getLength() const { return m_end - m_start; }
    
    void setContent(const std::string& content) { m_content = content; }
    void setType(PlaceholderType type) { m_type = type; }
    
    // 占位符验证
    bool isValid() const;
    bool isVariable() const;
    bool isFunction() const;
    bool isExpression() const;
    bool isBlock() const;

private:
    std::string m_content;
    PlaceholderType m_type;
    int m_start;
    int m_end;
};

/**
 * @brief 代码片段
 */
class CodeFragment {
public:
    CodeFragment(const std::string& content, int start, int end, bool isPlaceholder = false);
    ~CodeFragment() = default;

    std::string getContent() const { return m_content; }
    int getStart() const { return m_start; }
    int getEnd() const { return m_end; }
    int getLength() const { return m_end - m_start; }
    bool isPlaceholder() const { return m_isPlaceholder; }
    
    void setContent(const std::string& content) { m_content = content; }
    void setPlaceholder(bool isPlaceholder) { m_isPlaceholder = isPlaceholder; }
    
    // 片段验证
    bool isValid() const;
    bool isEmpty() const;

private:
    std::string m_content;
    int m_start;
    int m_end;
    bool m_isPlaceholder;
};

/**
 * @brief 统一扫描器
 */
class UnifiedScanner {
public:
    explicit UnifiedScanner(std::shared_ptr<CHTLContext> context);
    ~UnifiedScanner() = default;

    // 代码扫描
    std::vector<CodeFragment> scanCode(const std::string& code, ScanMode mode = ScanMode::BROAD);
    std::vector<Placeholder> scanPlaceholders(const std::string& code);
    
    // 代码切割
    std::vector<CodeFragment> cutCode(const std::string& code, const std::vector<std::string>& delimiters);
    std::vector<CodeFragment> cutCodeByPattern(const std::string& code, const std::string& pattern);
    
    // 智能增强
    std::string enhanceCode(const std::string& code, ScanMode mode = ScanMode::BROAD);
    std::string enhanceFragment(const CodeFragment& fragment);
    
    // 占位符机制
    std::string createPlaceholder(const std::string& content, PlaceholderType type);
    std::string replacePlaceholder(const std::string& code, const std::string& placeholder, const std::string& replacement);
    std::vector<std::string> extractPlaceholders(const std::string& code);
    
    // 边界处理
    std::string processBoundaries(const std::string& code);
    std::string fixBoundaries(const std::string& code);
    
    // 扫描配置
    void setScanMode(ScanMode mode) { m_scanMode = mode; }
    void setDelimiters(const std::vector<std::string>& delimiters) { m_delimiters = delimiters; }
    void setPlaceholderPatterns(const std::unordered_map<PlaceholderType, std::string>& patterns) { m_placeholderPatterns = patterns; }

private:
    std::shared_ptr<CHTLContext> m_context;
    ScanMode m_scanMode;
    std::vector<std::string> m_delimiters;
    std::unordered_map<PlaceholderType, std::string> m_placeholderPatterns;
    
    // 辅助方法
    std::vector<CodeFragment> scanCodeBroad(const std::string& code) const;
    std::vector<CodeFragment> scanCodeStrict(const std::string& code) const;
    std::vector<Placeholder> scanPlaceholdersBroad(const std::string& code) const;
    std::vector<Placeholder> scanPlaceholdersStrict(const std::string& code) const;
    
    PlaceholderType detectPlaceholderType(const std::string& content) const;
    std::string generatePlaceholderId(const std::string& content) const;
    bool isPlaceholderPattern(const std::string& content, PlaceholderType type) const;
    
    std::string processVariablePlaceholder(const std::string& content) const;
    std::string processFunctionPlaceholder(const std::string& content) const;
    std::string processExpressionPlaceholder(const std::string& content) const;
    std::string processBlockPlaceholder(const std::string& content) const;
    
    std::string fixSyntaxBoundaries(const std::string& code) const;
    std::string fixSemanticBoundaries(const std::string& code) const;
    std::string fixContextBoundaries(const std::string& code) const;
};

/**
 * @brief 代码切割器
 */
class CodeCutter {
public:
    explicit CodeCutter(std::shared_ptr<CHTLContext> context);
    ~CodeCutter() = default;

    // 代码切割
    std::vector<CodeFragment> cutByDelimiters(const std::string& code, const std::vector<std::string>& delimiters);
    std::vector<CodeFragment> cutByPattern(const std::string& code, const std::string& pattern);
    std::vector<CodeFragment> cutByLength(const std::string& code, int maxLength);
    
    // 智能切割
    std::vector<CodeFragment> smartCut(const std::string& code, int maxLength);
    std::vector<CodeFragment> cutBySyntax(const std::string& code);
    std::vector<CodeFragment> cutBySemantics(const std::string& code);
    
    // 切割验证
    bool validateCut(const std::vector<CodeFragment>& fragments) const;
    bool canCutAt(const std::string& code, int position) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::vector<CodeFragment> cutByDelimiter(const std::string& code, const std::string& delimiter) const;
    std::vector<CodeFragment> cutByRegex(const std::string& code, const std::regex& pattern) const;
    bool isCutPoint(const std::string& code, int position) const;
    bool isSyntaxBoundary(const std::string& code, int position) const;
    bool isSemanticBoundary(const std::string& code, int position) const;
};

/**
 * @brief 占位符管理器
 */
class PlaceholderManager {
public:
    explicit PlaceholderManager(std::shared_ptr<CHTLContext> context);
    ~PlaceholderManager() = default;

    // 占位符管理
    std::string createPlaceholder(const std::string& content, PlaceholderType type);
    std::string getPlaceholderContent(const std::string& placeholder) const;
    PlaceholderType getPlaceholderType(const std::string& placeholder) const;
    bool isPlaceholder(const std::string& content) const;
    
    // 占位符替换
    std::string replacePlaceholder(const std::string& code, const std::string& placeholder, const std::string& replacement);
    std::string replaceAllPlaceholders(const std::string& code, const std::unordered_map<std::string, std::string>& replacements);
    
    // 占位符提取
    std::vector<std::string> extractPlaceholders(const std::string& code) const;
    std::vector<Placeholder> extractPlaceholderObjects(const std::string& code) const;
    
    // 占位符验证
    bool validatePlaceholder(const std::string& placeholder) const;
    bool validatePlaceholders(const std::string& code) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::string> m_placeholderContent;
    std::unordered_map<std::string, PlaceholderType> m_placeholderTypes;
    int m_placeholderCounter;
    
    // 辅助方法
    std::string generatePlaceholderId() const;
    std::string formatPlaceholder(const std::string& id, PlaceholderType type) const;
    PlaceholderType parsePlaceholderType(const std::string& placeholder) const;
    std::string parsePlaceholderId(const std::string& placeholder) const;
};

/**
 * @brief 边界处理器
 */
class BoundaryProcessor {
public:
    explicit BoundaryProcessor(std::shared_ptr<CHTLContext> context);
    ~BoundaryProcessor() = default;

    // 边界处理
    std::string processBoundaries(const std::string& code);
    std::string fixSyntaxBoundaries(const std::string& code);
    std::string fixSemanticBoundaries(const std::string& code);
    std::string fixContextBoundaries(const std::string& code);
    
    // 边界检测
    bool hasSyntaxBoundary(const std::string& code, int position) const;
    bool hasSemanticBoundary(const std::string& code, int position) const;
    bool hasContextBoundary(const std::string& code, int position) const;
    
    // 边界修复
    std::string fixBoundaryAt(const std::string& code, int position) const;
    std::string insertBoundary(const std::string& code, int position, const std::string& boundary) const;
    std::string removeBoundary(const std::string& code, int position) const;

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    bool isSyntaxBoundaryChar(char c) const;
    bool isSemanticBoundaryChar(char c) const;
    bool isContextBoundaryChar(char c) const;
    std::string getBoundaryChar(char c) const;
    std::string getBoundaryString(const std::string& code, int position) const;
};

} // namespace CHTL