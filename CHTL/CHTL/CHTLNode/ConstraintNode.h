#pragma once

#include "BaseNode.h"

namespace CHTL {

enum class ConstraintType {
    PRECISE,        // 精确约束
    TYPE,           // 类型约束
    GLOBAL          // 全局约束
};

enum class ConstraintTarget {
    ELEMENT,        // 元素
    ATTRIBUTE,      // 属性
    STYLE,          // 样式
    SCRIPT,         // 脚本
    TEMPLATE,       // 模板
    CUSTOM,         // 自定义
    NAMESPACE,      // 命名空间
    ALL             // 所有
};

class ConstraintNode : public BaseNode {
private:
    ConstraintType constraintType;
    ConstraintTarget target;
    std::string constraintExpression;
    std::vector<std::string> exceptList;
    std::map<std::string, std::string> constraintRules;
    
    // 约束配置
    bool isEnabled;
    std::string scope;
    int priority;
    
public:
    ConstraintNode(ConstraintType type, ConstraintTarget target, const std::string& expression);
    virtual ~ConstraintNode() = default;
    
    // 约束类型
    ConstraintType getConstraintType() const { return constraintType; }
    void setConstraintType(ConstraintType type) { constraintType = type; }
    
    // 约束目标
    ConstraintTarget getTarget() const { return target; }
    void setTarget(ConstraintTarget target) { this->target = target; }
    
    // 约束表达式
    const std::string& getConstraintExpression() const { return constraintExpression; }
    void setConstraintExpression(const std::string& expression) { constraintExpression = expression; }
    
    // 例外列表
    void addExcept(const std::string& exception);
    void removeExcept(const std::string& exception);
    const std::vector<std::string>& getExceptList() const { return exceptList; }
    
    // 约束规则
    void addRule(const std::string& rule, const std::string& value);
    void removeRule(const std::string& rule);
    std::string getRule(const std::string& rule) const;
    bool hasRule(const std::string& rule) const;
    const std::map<std::string, std::string>& getRules() const { return constraintRules; }
    
    // 配置选项
    void setEnabled(bool enabled) { isEnabled = enabled; }
    bool isConstraintEnabled() const { return isEnabled; }
    
    void setScope(const std::string& scope) { this->scope = scope; }
    const std::string& getScope() const { return scope; }
    
    void setPriority(int priority) { this->priority = priority; }
    int getPriority() const { return priority; }
    
    // 约束验证
    bool validate(const std::string& target) const;
    bool validateElement(const std::string& elementName) const;
    bool validateAttribute(const std::string& attributeName) const;
    bool validateStyle(const std::string& styleName) const;
    bool validateScript(const std::string& scriptName) const;
    
    // 约束应用
    bool appliesTo(const std::string& target) const;
    bool appliesToElement(const std::string& elementName) const;
    bool appliesToAttribute(const std::string& attributeName) const;
    bool appliesToStyle(const std::string& styleName) const;
    bool appliesToScript(const std::string& scriptName) const;
    
    // 约束解析
    std::vector<std::string> parseConstraintExpression() const;
    bool evaluateConstraint(const std::string& target) const;
    
    // 克隆
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 序列化
    virtual std::string toString() const override;
    virtual std::string toHTML() const override;
    virtual std::string toCSS() const override;
    virtual std::string toJS() const override;
    
    // 验证
    virtual bool isValid() const override;
    
    // 调试
    virtual std::string debugString() const override;
    
    // 静态方法
    static ConstraintType parseConstraintType(const std::string& type);
    static ConstraintTarget parseConstraintTarget(const std::string& target);
    static std::string constraintTypeToString(ConstraintType type);
    static std::string constraintTargetToString(ConstraintTarget target);
    
private:
    bool isInExceptList(const std::string& target) const;
    bool matchesConstraintExpression(const std::string& target) const;
    std::vector<std::string> tokenizeExpression(const std::string& expression) const;
    bool evaluateToken(const std::string& token, const std::string& target) const;
};

} // namespace CHTL