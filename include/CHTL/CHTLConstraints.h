#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <set>

namespace CHTL {

/**
 * @brief 约束类型枚举
 */
enum class ConstraintType {
    EXACT,          // 精确约束
    TYPE,           // 类型约束
    GLOBAL          // 全局约束
};

/**
 * @brief 约束目标类型枚举
 */
enum class ConstraintTargetType {
    HTML_ELEMENT,   // HTML 元素
    TEMPLATE,       // 模板
    CUSTOM,         // 自定义
    ORIGIN,         // 原始嵌入
    UNKNOWN         // 未知
};

/**
 * @brief 约束规则
 */
class ConstraintRule {
public:
    ConstraintRule(ConstraintType type, const std::string& target, 
                   ConstraintTargetType targetType, int line = 0, int column = 0);
    ~ConstraintRule() = default;

    ConstraintType getType() const { return m_type; }
    std::string getTarget() const { return m_target; }
    ConstraintTargetType getTargetType() const { return m_targetType; }
    int getLine() const { return m_line; }
    int getColumn() const { return m_column; }
    
    void setTarget(const std::string& target) { m_target = target; }
    void setTargetType(ConstraintTargetType targetType) { m_targetType = targetType; }
    
    // 约束检查
    bool matches(const std::string& name, ConstraintTargetType type) const;
    bool isExactMatch(const std::string& name) const;
    bool isTypeMatch(ConstraintTargetType type) const;

private:
    ConstraintType m_type;
    std::string m_target;
    ConstraintTargetType m_targetType;
    int m_line;
    int m_column;
};

/**
 * @brief 约束管理器
 */
class ConstraintManager {
public:
    explicit ConstraintManager(std::shared_ptr<CHTLContext> context);
    ~ConstraintManager() = default;

    // 约束注册
    void addConstraint(std::shared_ptr<ConstraintRule> constraint);
    void addConstraint(ConstraintType type, const std::string& target, 
                      ConstraintTargetType targetType, int line = 0, int column = 0);
    
    // 约束检查
    bool isAllowed(const std::string& name, ConstraintTargetType type) const;
    bool isExactAllowed(const std::string& name) const;
    bool isTypeAllowed(ConstraintTargetType type) const;
    
    // 约束查询
    std::vector<std::shared_ptr<ConstraintRule>> getConstraints(ConstraintType type) const;
    std::vector<std::shared_ptr<ConstraintRule>> getAllConstraints() const;
    
    // 约束应用
    void applyConstraints(std::shared_ptr<BaseNode> root);
    void applyConstraintsToNode(std::shared_ptr<BaseNode> node);
    
    // 清理
    void clear();
    void removeConstraint(const std::string& target);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::vector<std::shared_ptr<ConstraintRule>> m_constraints;
    std::unordered_map<ConstraintType, std::vector<std::shared_ptr<ConstraintRule>>> m_constraintsByType;
    
    // 辅助方法
    ConstraintTargetType parseTargetType(const std::string& typeString) const;
    bool checkExactConstraints(const std::string& name) const;
    bool checkTypeConstraints(ConstraintTargetType type) const;
    bool checkGlobalConstraints(const std::string& name, ConstraintTargetType type) const;
};

/**
 * @brief 约束解析器
 */
class ConstraintParser {
public:
    explicit ConstraintParser(std::shared_ptr<CHTLContext> context);
    ~ConstraintParser() = default;

    // 解析约束
    std::shared_ptr<ConstraintRule> parseConstraint(std::shared_ptr<BaseNode> node);
    std::vector<std::shared_ptr<ConstraintRule>> parseConstraints(std::shared_ptr<BaseNode> root);
    
    // 解析 except 语句
    std::vector<std::shared_ptr<ConstraintRule>> parseExceptStatement(std::shared_ptr<BaseNode> node);

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    ConstraintType parseConstraintType(const std::string& typeString) const;
    ConstraintTargetType parseTargetType(const std::string& typeString) const;
    std::vector<std::string> parseTargetList(const std::string& targetString) const;
};

/**
 * @brief 约束验证器
 */
class ConstraintValidator {
public:
    explicit ConstraintValidator(std::shared_ptr<CHTLContext> context);
    ~ConstraintValidator() = default;

    // 验证约束
    bool validateConstraints(std::shared_ptr<BaseNode> root);
    bool validateNode(std::shared_ptr<BaseNode> node);
    
    // 验证特定约束
    bool validateExactConstraints(std::shared_ptr<BaseNode> node);
    bool validateTypeConstraints(std::shared_ptr<BaseNode> node);
    bool validateGlobalConstraints(std::shared_ptr<BaseNode> node);
    
    // 错误报告
    void reportConstraintViolation(const std::string& name, ConstraintTargetType type, 
                                  const std::string& constraint, int line, int column);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unique_ptr<ConstraintManager> m_constraintManager;
    std::unique_ptr<ConstraintParser> m_parser;
    
    // 辅助方法
    bool isElementAllowed(const std::string& tagName) const;
    bool isTemplateAllowed(const std::string& templateName) const;
    bool isCustomAllowed(const std::string& customName) const;
    bool isOriginAllowed(const std::string& originName) const;
};

/**
 * @brief 约束处理器
 */
class ConstraintProcessor {
public:
    explicit ConstraintProcessor(std::shared_ptr<CHTLContext> context);
    ~ConstraintProcessor() = default;

    // 处理约束
    void processConstraints(std::shared_ptr<BaseNode> root);
    
    // 应用约束
    void applyConstraints(std::shared_ptr<BaseNode> root);
    
    // 验证约束
    bool validateConstraints(std::shared_ptr<BaseNode> root);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unique_ptr<ConstraintManager> m_constraintManager;
    std::unique_ptr<ConstraintParser> m_parser;
    std::unique_ptr<ConstraintValidator> m_validator;
    
    // 辅助方法
    void processNodeConstraints(std::shared_ptr<BaseNode> node);
    void processElementConstraints(std::shared_ptr<ElementNode> node);
    void processTemplateConstraints(std::shared_ptr<TemplateNode> node);
    void processCustomConstraints(std::shared_ptr<CustomNode> node);
    void processOriginConstraints(std::shared_ptr<OriginNode> node);
};

} // namespace CHTL