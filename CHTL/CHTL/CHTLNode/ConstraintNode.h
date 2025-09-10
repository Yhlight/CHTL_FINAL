#ifndef CHTL_CONSTRAINT_NODE_H
#define CHTL_CONSTRAINT_NODE_H

#include "BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

class ConstraintNode : public BaseNode {
public:
    // 约束类型
    enum class ConstraintType {
        EXACT,      // 精确约束
        TYPE,       // 类型约束
        GLOBAL,     // 全局约束
        LOCAL,      // 局部约束
        INHERITED   // 继承约束
    };
    
    // 约束操作
    enum class ConstraintOperation {
        ALLOW,      // 允许
        DENY,       // 拒绝
        REQUIRE,    // 要求
        OPTIONAL,   // 可选
        WARN        // 警告
    };

private:
    ConstraintType constraintType;
    std::string constraintName;
    std::string constraintExpression;
    std::map<std::string, std::string> constraintParameters;
    
    // 约束目标
    std::vector<std::string> targetElements;
    std::vector<std::string> targetProperties;
    std::vector<std::string> targetTypes;
    
    // 约束条件
    std::vector<std::string> conditions;
    std::map<std::string, std::string> conditionValues;
    std::map<std::string, std::function<bool(const std::string&)>> conditionFunctions;
    
    ConstraintOperation operation;
    std::string operationMessage;
    int operationPriority;
    
    // 约束状态
    bool isActive;
    bool isProcessed;
    bool isViolated;
    std::vector<std::string> violationMessages;
    std::vector<std::string> constraintErrors;
    std::vector<std::string> constraintWarnings;
    
    // 约束元数据
    std::map<std::string, std::string> metadata;
    int priority;
    bool isRequired;
    bool isOptional;

public:
    ConstraintNode(const std::string& name, ConstraintType type = ConstraintType::EXACT);
    virtual ~ConstraintNode() = default;
    
    // 约束类型
    ConstraintType getConstraintType() const { return constraintType; }
    void setConstraintType(ConstraintType type) { constraintType = type; }
    std::string getConstraintTypeName() const;
    
    // 约束名称
    const std::string& getConstraintName() const { return constraintName; }
    void setConstraintName(const std::string& name) { constraintName = name; }
    
    // 约束表达式
    const std::string& getConstraintExpression() const { return constraintExpression; }
    void setConstraintExpression(const std::string& expression) { constraintExpression = expression; }
    
    // 约束参数
    void setConstraintParameter(const std::string& key, const std::string& value);
    std::string getConstraintParameter(const std::string& key) const;
    bool hasConstraintParameter(const std::string& key) const;
    void removeConstraintParameter(const std::string& key);
    const std::map<std::string, std::string>& getConstraintParameters() const { return constraintParameters; }
    
    // 约束目标
    void addTargetElement(const std::string& element);
    void removeTargetElement(const std::string& element);
    const std::vector<std::string>& getTargetElements() const { return targetElements; }
    bool hasTargetElement(const std::string& element) const;
    
    void addTargetProperty(const std::string& property);
    void removeTargetProperty(const std::string& property);
    const std::vector<std::string>& getTargetProperties() const { return targetProperties; }
    bool hasTargetProperty(const std::string& property) const;
    
    void addTargetType(const std::string& type);
    void removeTargetType(const std::string& type);
    const std::vector<std::string>& getTargetTypes() const { return targetTypes; }
    bool hasTargetType(const std::string& type) const;
    
    // 约束条件
    void addCondition(const std::string& condition);
    void removeCondition(const std::string& condition);
    const std::vector<std::string>& getConditions() const { return conditions; }
    bool hasCondition(const std::string& condition) const;
    
    void setConditionValue(const std::string& condition, const std::string& value);
    std::string getConditionValue(const std::string& condition) const;
    bool hasConditionValue(const std::string& condition) const;
    void removeConditionValue(const std::string& condition);
    const std::map<std::string, std::string>& getConditionValues() const { return conditionValues; }
    
    void setConditionFunction(const std::string& condition, std::function<bool(const std::string&)> function);
    std::function<bool(const std::string&)> getConditionFunction(const std::string& condition) const;
    bool hasConditionFunction(const std::string& condition) const;
    void removeConditionFunction(const std::string& condition);
    
    // 约束操作
    ConstraintOperation getOperation() const { return operation; }
    void setOperation(ConstraintOperation op) { operation = op; }
    std::string getOperationName() const;
    
    const std::string& getOperationMessage() const { return operationMessage; }
    void setOperationMessage(const std::string& message) { operationMessage = message; }
    
    int getOperationPriority() const { return operationPriority; }
    void setOperationPriority(int priority) { operationPriority = priority; }
    
    // 约束状态
    bool isActiveConstraint() const { return isActive; }
    void setActive(bool active) { isActive = active; }
    
    bool isProcessedConstraint() const { return isProcessed; }
    void setProcessed(bool processed) { isProcessed = processed; }
    
    bool isViolatedConstraint() const { return isViolated; }
    void setViolated(bool violated) { isViolated = violated; }
    
    // 约束错误和警告
    const std::vector<std::string>& getConstraintErrors() const { return constraintErrors; }
    const std::vector<std::string>& getConstraintWarnings() const { return constraintWarnings; }
    void addConstraintError(const std::string& error);
    void addConstraintWarning(const std::string& warning);
    void clearConstraintMessages();
    
    // 约束违反
    const std::vector<std::string>& getViolationMessages() const { return violationMessages; }
    void addViolationMessage(const std::string& message);
    void clearViolationMessages();
    
    // 约束元数据
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    const std::map<std::string, std::string>& getMetadata() const { return metadata; }
    
    // 约束特性
    int getPriority() const { return priority; }
    void setPriority(int pri) { priority = pri; }
    
    bool isRequiredConstraint() const { return isRequired; }
    void setRequired(bool required) { isRequired = required; }
    
    bool isOptionalConstraint() const { return isOptional; }
    void setOptional(bool optional) { isOptional = optional; }
    
    // 约束处理
    bool processConstraint();
    bool processExactConstraint();
    bool processTypeConstraint();
    bool processGlobalConstraint();
    bool processLocalConstraint();
    bool processInheritedConstraint();
    
    // 约束验证
    bool validateConstraint() const;
    bool validateExpression() const;
    bool validateTargets() const;
    bool validateConditions() const;
    bool validateOperation() const;
    
    // 约束检查
    bool checkConstraint(const std::string& value) const;
    bool checkElementConstraint(const std::string& element) const;
    bool checkPropertyConstraint(const std::string& property, const std::string& value) const;
    bool checkTypeConstraint(const std::string& type, const std::string& value) const;
    
    // 约束转换
    std::string toHTML() const override;
    std::string toCSS() const;
    std::string toJavaScript() const;
    std::string toConstraintString() const;
    
    // 约束格式化
    std::string formatConstraint() const;
    std::string minifyConstraint() const;
    std::string beautifyConstraint() const;
    
    // 约束压缩
    std::string compressConstraint() const;
    std::string decompressConstraint() const;
    
    // 约束编码
    std::string encodeConstraint() const;
    std::string decodeConstraint() const;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const ConstraintNode& other) const;
    bool operator!=(const ConstraintNode& other) const;

private:
    // 辅助方法
    std::string processConstraintExpression(const std::string& expression) const;
    std::string validateConstraintExpression(const std::string& expression) const;
    std::string sanitizeConstraintExpression(const std::string& expression) const;
    
    // 表达式处理
    std::string parseConstraintExpression(const std::string& expression) const;
    std::string evaluateConstraintExpression(const std::string& expression) const;
    std::string optimizeConstraintExpression(const std::string& expression) const;
    
    // 条件处理
    bool evaluateCondition(const std::string& condition, const std::string& value) const;
    bool evaluateConditionFunction(const std::string& condition, const std::string& value) const;
    bool evaluateConditionValue(const std::string& condition, const std::string& value) const;
    
    // 约束处理
    void processConstraintTargets();
    void processConstraintConditions();
    void processConstraintOperation();
    
    // 约束验证
    bool validateConstraintTargets() const;
    bool validateConstraintConditions() const;
    bool validateConstraintOperation() const;
    
    // 约束检查
    bool checkExactConstraint(const std::string& value) const;
    bool checkGlobalConstraint(const std::string& value) const;
    bool checkLocalConstraint(const std::string& value) const;
    bool checkInheritedConstraint(const std::string& value) const;
    
    // 错误处理
    void handleConstraintError(const std::string& error);
    void handleConstraintWarning(const std::string& warning);
    void handleConstraintViolation(const std::string& violation);
};

} // namespace CHTL

#endif // CHTL_CONSTRAINT_NODE_H