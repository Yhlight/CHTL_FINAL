#pragma once

#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLNode.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

/**
 * @brief CHTL高级解析器
 * 支持属性条件表达式、约束系统、配置组等高级特性
 */
class CHTLAdvancedParser : public CHTLParser {
public:
    CHTLAdvancedParser();
    ~CHTLAdvancedParser() = default;

    /**
     * @brief 解析属性条件表达式
     * @param expression 表达式字符串
     * @return 解析结果
     */
    std::string parseConditionalExpression(const std::string& expression);
    
    /**
     * @brief 解析约束系统
     * @param node 当前节点
     * @param constraint 约束字符串
     */
    void parseConstraint(std::shared_ptr<CHTLNode> node, const std::string& constraint);
    
    /**
     * @brief 解析配置组
     * @param configContent 配置内容
     * @return 配置节点
     */
    std::shared_ptr<CHTLNode> parseConfiguration(const std::string& configContent);
    
    /**
     * @brief 解析命名空间
     * @param namespaceContent 命名空间内容
     * @return 命名空间节点
     */
    std::shared_ptr<CHTLNode> parseNamespace(const std::string& namespaceContent);
    
    /**
     * @brief 解析模板继承
     * @param templateNode 模板节点
     * @param inheritanceContent 继承内容
     */
    void parseTemplateInheritance(std::shared_ptr<TemplateNode> templateNode, 
                                 const std::string& inheritanceContent);
    
    /**
     * @brief 解析自定义特例化
     * @param customNode 自定义节点
     * @param specializationContent 特例化内容
     */
    void parseCustomSpecialization(std::shared_ptr<CHTLNode> customNode, 
                                  const std::string& specializationContent);

private:
    // 属性条件表达式解析
    std::string parseLogicalExpression(const std::string& expression);
    std::string parseComparisonExpression(const std::string& expression);
    std::string parseArithmeticExpression(const std::string& expression);
    std::string parsePropertyReference(const std::string& reference);
    
    // 约束解析
    void parsePreciseConstraint(std::shared_ptr<CHTLNode> node, const std::string& constraint);
    void parseTypeConstraint(std::shared_ptr<CHTLNode> node, const std::string& constraint);
    void parseGlobalConstraint(std::shared_ptr<CHTLNode> node, const std::string& constraint);
    
    // 配置解析
    void parseNameConfiguration(std::shared_ptr<CHTLNode> configNode, 
                               const std::string& nameContent);
    void parseOriginTypeConfiguration(std::shared_ptr<CHTLNode> configNode, 
                                     const std::string& originTypeContent);
    
    // 模板解析
    void parseTemplateVariables(std::shared_ptr<TemplateNode> templateNode, 
                               const std::string& variablesContent);
    void parseTemplateElements(std::shared_ptr<TemplateNode> templateNode, 
                              const std::string& elementsContent);
    
    // 自定义解析
    void parseCustomDeletion(std::shared_ptr<CHTLNode> customNode, 
                            const std::string& deletionContent);
    void parseCustomInsertion(std::shared_ptr<CHTLNode> customNode, 
                             const std::string& insertionContent);
    void parseCustomIndexAccess(std::shared_ptr<CHTLNode> customNode, 
                               const std::string& indexContent);
    
    // 工具函数
    bool isPropertyReference(const std::string& reference) const;
    bool isConstraintKeyword(const std::string& keyword) const;
    bool isConfigurationKeyword(const std::string& keyword) const;
    bool isTemplateKeyword(const std::string& keyword) const;
    bool isCustomKeyword(const std::string& keyword) const;
    
    // 表达式计算
    std::string evaluateExpression(const std::string& expression);
    std::string evaluateComparison(const std::string& left, const std::string& op, const std::string& right);
    std::string evaluateArithmetic(const std::string& left, const std::string& op, const std::string& right);
    
    // 属性解析
    std::string parsePropertyValue(const std::string& value);
    std::string parsePropertyUnit(const std::string& value);
    std::string parsePropertyColor(const std::string& value);
    std::string parsePropertySize(const std::string& value);
    
    // 选择器解析
    std::string parseSelector(const std::string& selector);
    std::string parseClassSelector(const std::string& selector);
    std::string parseIdSelector(const std::string& selector);
    std::string parseTagSelector(const std::string& selector);
    std::string parseDescendantSelector(const std::string& selector);
    std::string parseIndexSelector(const std::string& selector);
};

/**
 * @brief 属性条件表达式计算器
 */
class ConditionalExpressionCalculator {
public:
    ConditionalExpressionCalculator();
    ~ConditionalExpressionCalculator() = default;
    
    /**
     * @brief 计算条件表达式
     * @param expression 表达式
     * @param context 上下文属性
     * @return 计算结果
     */
    std::string calculate(const std::string& expression, 
                         const std::unordered_map<std::string, std::string>& context);
    
    /**
     * @brief 设置属性值
     * @param property 属性名
     * @param value 属性值
     */
    void setProperty(const std::string& property, const std::string& value);
    
    /**
     * @brief 获取属性值
     * @param property 属性名
     * @return 属性值
     */
    std::string getProperty(const std::string& property) const;

private:
    std::unordered_map<std::string, std::string> m_properties;
    
    std::string parseExpression(const std::string& expression);
    std::string parseLogicalOr(const std::string& expression);
    std::string parseLogicalAnd(const std::string& expression);
    std::string parseEquality(const std::string& expression);
    std::string parseComparison(const std::string& expression);
    std::string parseAddition(const std::string& expression);
    std::string parseMultiplication(const std::string& expression);
    std::string parsePrimary(const std::string& expression);
    
    bool isOperator(const std::string& op) const;
    int getOperatorPrecedence(const std::string& op) const;
    std::string evaluateOperator(const std::string& left, const std::string& op, const std::string& right);
    
    std::string convertToPixels(const std::string& value) const;
    std::string convertToNumber(const std::string& value) const;
    bool isNumeric(const std::string& value) const;
    bool isColor(const std::string& value) const;
    bool isSize(const std::string& value) const;
};

/**
 * @brief 约束系统管理器
 */
class ConstraintManager {
public:
    ConstraintManager();
    ~ConstraintManager() = default;
    
    /**
     * @brief 添加约束
     * @param node 节点
     * @param constraint 约束
     */
    void addConstraint(std::shared_ptr<CHTLNode> node, const std::string& constraint);
    
    /**
     * @brief 检查约束
     * @param node 节点
     * @param child 子节点
     * @return 是否违反约束
     */
    bool checkConstraint(std::shared_ptr<CHTLNode> node, std::shared_ptr<CHTLNode> child) const;
    
    /**
     * @brief 获取约束
     * @param node 节点
     * @return 约束列表
     */
    std::vector<std::string> getConstraints(std::shared_ptr<CHTLNode> node) const;
    
    /**
     * @brief 清除约束
     * @param node 节点
     */
    void clearConstraints(std::shared_ptr<CHTLNode> node);

private:
    std::unordered_map<std::shared_ptr<CHTLNode>, std::vector<std::string>> m_constraints;
    
    bool isElementConstraint(const std::string& constraint) const;
    bool isTypeConstraint(const std::string& constraint) const;
    bool isGlobalConstraint(const std::string& constraint) const;
    
    bool checkElementConstraint(std::shared_ptr<CHTLNode> node, const std::string& constraint) const;
    bool checkTypeConstraint(std::shared_ptr<CHTLNode> node, const std::string& constraint) const;
    bool checkGlobalConstraint(std::shared_ptr<CHTLNode> node, const std::string& constraint) const;
};

} // namespace CHTL