#pragma once

#include "CHTLNode/BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>

namespace CHTL {

enum class ValidationLevel {
    SYNTAX,     // 语法验证
    SEMANTIC,   // 语义验证
    STYLE,      // 样式验证
    PERFORMANCE // 性能验证
};

enum class ValidationSeverity {
    ERROR,      // 错误
    WARNING,    // 警告
    INFO,       // 信息
    HINT        // 提示
};

struct ValidationIssue {
    ValidationSeverity severity;
    std::string message;
    std::string suggestion;
    size_t line;
    size_t column;
    std::string context;
    
    ValidationIssue(ValidationSeverity sev, const std::string& msg, 
                   size_t l, size_t c, const std::string& ctx = "")
        : severity(sev), message(msg), line(l), column(c), context(ctx) {}
};

class CHTLValidator {
private:
    std::vector<ValidationIssue> issues;
    std::map<std::string, std::shared_ptr<BaseNode>> templates;
    std::map<std::string, std::shared_ptr<BaseNode>> customs;
    std::map<std::string, std::string> variables;
    std::set<std::string> usedIdentifiers;
    std::set<std::string> definedIdentifiers;
    
    // 验证选项
    bool strictMode;
    bool enableWarnings;
    bool enableHints;
    ValidationLevel maxLevel;
    
    // 验证方法
    void validateNode(std::shared_ptr<BaseNode> node);
    void validateElement(std::shared_ptr<BaseNode> node);
    void validateTemplate(std::shared_ptr<BaseNode> node);
    void validateCustom(std::shared_ptr<BaseNode> node);
    void validateStyle(std::shared_ptr<BaseNode> node);
    void validateScript(std::shared_ptr<BaseNode> node);
    void validateImport(std::shared_ptr<BaseNode> node);
    void validateNamespace(std::shared_ptr<BaseNode> node);
    void validateConfiguration(std::shared_ptr<BaseNode> node);
    
    // 语法验证
    void validateSyntax(std::shared_ptr<BaseNode> node);
    void validateElementSyntax(std::shared_ptr<BaseNode> node);
    void validateAttributeSyntax(std::shared_ptr<BaseNode> node);
    void validateTemplateSyntax(std::shared_ptr<BaseNode> node);
    
    // 语义验证
    void validateSemantics(std::shared_ptr<BaseNode> node);
    void validateReferences(std::shared_ptr<BaseNode> node);
    void validateInheritance(std::shared_ptr<BaseNode> node);
    void validateScope(std::shared_ptr<BaseNode> node);
    
    // 样式验证
    void validateStyleRules(std::shared_ptr<BaseNode> node);
    void validateCSSProperties(std::shared_ptr<BaseNode> node);
    void validateSelectors(std::shared_ptr<BaseNode> node);
    void validateMediaQueries(std::shared_ptr<BaseNode> node);
    
    // 性能验证
    void validatePerformance(std::shared_ptr<BaseNode> node);
    void validateComplexity(std::shared_ptr<BaseNode> node);
    void validateRedundancy(std::shared_ptr<BaseNode> node);
    
    // 辅助方法
    bool isValidElementName(const std::string& name) const;
    bool isValidAttributeName(const std::string& name) const;
    bool isValidTemplateName(const std::string& name) const;
    bool isValidVariableName(const std::string& name) const;
    bool isValidCSSProperty(const std::string& property) const;
    bool isValidCSSValue(const std::string& value) const;
    
    void addIssue(ValidationSeverity severity, const std::string& message, 
                 size_t line, size_t column, const std::string& context = "");
    void addIssue(ValidationSeverity severity, const std::string& message, 
                 std::shared_ptr<BaseNode> node, const std::string& context = "");
    
    // 上下文管理
    void pushContext(std::shared_ptr<BaseNode> node);
    void popContext();
    std::shared_ptr<BaseNode> getCurrentContext() const;
    
    std::vector<std::shared_ptr<BaseNode>> contextStack;
    
public:
    CHTLValidator();
    CHTLValidator(bool strict, bool warnings = true, bool hints = true, 
                 ValidationLevel maxLevel = ValidationLevel::PERFORMANCE);
    ~CHTLValidator() = default;
    
    // 主要验证方法
    std::vector<ValidationIssue> validate(std::shared_ptr<BaseNode> ast);
    std::vector<ValidationIssue> validate(const std::string& source);
    
    // 分类验证
    std::vector<ValidationIssue> validateSyntax(std::shared_ptr<BaseNode> ast);
    std::vector<ValidationIssue> validateSemantics(std::shared_ptr<BaseNode> ast);
    std::vector<ValidationIssue> validateStyle(std::shared_ptr<BaseNode> ast);
    std::vector<ValidationIssue> validatePerformance(std::shared_ptr<BaseNode> ast);
    
    // 问题管理
    void clearIssues();
    size_t getIssueCount() const;
    size_t getErrorCount() const;
    size_t getWarningCount() const;
    std::vector<ValidationIssue> getIssues() const;
    std::vector<ValidationIssue> getIssues(ValidationSeverity severity) const;
    
    // 选项设置
    void setStrictMode(bool strict) { strictMode = strict; }
    void setEnableWarnings(bool enable) { enableWarnings = enable; }
    void setEnableHints(bool enable) { enableHints = enable; }
    void setMaxLevel(ValidationLevel level) { maxLevel = level; }
    
    // 统计信息
    struct ValidationStats {
        size_t totalIssues;
        size_t errorCount;
        size_t warningCount;
        size_t infoCount;
        size_t hintCount;
        double validationTime;
    };
    
    ValidationStats getStats() const;
    void resetStats();
    
    // 调试
    void enableDebugMode(bool enable = true);
    std::string getDebugInfo() const;
    
private:
    bool debugMode;
    ValidationStats stats;
};

} // namespace CHTL