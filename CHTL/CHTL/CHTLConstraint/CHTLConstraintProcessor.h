#ifndef CHTL_CONSTRAINT_PROCESSOR_H
#define CHTL_CONSTRAINT_PROCESSOR_H

#include "../CHTLNode/ConstraintNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <set>

namespace CHTL {

class CHTLConstraintProcessor {
private:
    // 约束处理器状态
    bool debugMode;
    bool strictMode;
    bool enableConstraints;
    bool enableValidation;
    bool enableWarnings;
    bool enableErrors;
    
    // 约束存储
    std::vector<std::shared_ptr<ConstraintNode>> constraints;
    std::map<std::string, std::shared_ptr<ConstraintNode>> constraintMap;
    std::map<std::string, std::vector<std::shared_ptr<ConstraintNode>>> constraintGroups;
    
    // 约束类型映射
    std::map<std::string, ConstraintNode::ConstraintType> constraintTypeMap;
    std::map<ConstraintNode::ConstraintType, std::string> constraintTypeNames;
    
    // 约束操作映射
    std::map<std::string, ConstraintNode::ConstraintOperation> constraintOperationMap;
    std::map<ConstraintNode::ConstraintOperation, std::string> constraintOperationNames;
    
    // 约束目标映射
    std::map<std::string, std::vector<std::shared_ptr<ConstraintNode>>> elementConstraints;
    std::map<std::string, std::vector<std::shared_ptr<ConstraintNode>>> propertyConstraints;
    std::map<std::string, std::vector<std::shared_ptr<ConstraintNode>>> typeConstraints;
    
    // 约束条件映射
    std::map<std::string, std::function<bool(const std::string&)>> conditionFunctions;
    std::map<std::string, std::string> conditionValues;
    
    // 约束验证
    std::vector<std::string> validationErrors;
    std::vector<std::string> validationWarnings;
    std::vector<std::string> constraintViolations;
    
    // 约束统计
    int totalConstraints;
    int activeConstraints;
    int processedConstraints;
    int violatedConstraints;
    int errorConstraints;
    int warningConstraints;
    
    // 约束配置
    std::map<std::string, std::string> constraintConfig;
    std::map<std::string, bool> constraintFlags;
    std::map<std::string, int> constraintLimits;
    
    // 约束优先级
    std::map<std::string, int> constraintPriorities;
    std::map<int, std::vector<std::shared_ptr<ConstraintNode>>> priorityConstraints;

public:
    CHTLConstraintProcessor();
    virtual ~CHTLConstraintProcessor() = default;
    
    // 配置方法
    void setDebugMode(bool debug) { debugMode = debug; }
    bool isDebugMode() const { return debugMode; }
    
    void setStrictMode(bool strict) { strictMode = strict; }
    bool isStrictMode() const { return strictMode; }
    
    void setEnableConstraints(bool enable) { enableConstraints = enable; }
    bool isConstraintsEnabled() const { return enableConstraints; }
    
    void setEnableValidation(bool enable) { enableValidation = enable; }
    bool isValidationEnabled() const { return enableValidation; }
    
    void setEnableWarnings(bool enable) { enableWarnings = enable; }
    bool isWarningsEnabled() const { return enableWarnings; }
    
    void setEnableErrors(bool enable) { enableErrors = enable; }
    bool isErrorsEnabled() const { return enableErrors; }
    
    // 约束管理
    void addConstraint(std::shared_ptr<ConstraintNode> constraint);
    void removeConstraint(const std::string& name);
    void removeConstraint(std::shared_ptr<ConstraintNode> constraint);
    std::shared_ptr<ConstraintNode> getConstraint(const std::string& name) const;
    std::vector<std::shared_ptr<ConstraintNode>> getConstraints() const;
    std::vector<std::shared_ptr<ConstraintNode>> getConstraintsByType(ConstraintNode::ConstraintType type) const;
    std::vector<std::shared_ptr<ConstraintNode>> getConstraintsByOperation(ConstraintNode::ConstraintOperation operation) const;
    
    // 约束组管理
    void addConstraintToGroup(const std::string& groupName, std::shared_ptr<ConstraintNode> constraint);
    void removeConstraintFromGroup(const std::string& groupName, std::shared_ptr<ConstraintNode> constraint);
    std::vector<std::shared_ptr<ConstraintNode>> getConstraintsByGroup(const std::string& groupName) const;
    std::vector<std::string> getConstraintGroups() const;
    
    // 约束类型管理
    void registerConstraintType(const std::string& name, ConstraintNode::ConstraintType type);
    void unregisterConstraintType(const std::string& name);
    ConstraintNode::ConstraintType getConstraintType(const std::string& name) const;
    std::string getConstraintTypeName(ConstraintNode::ConstraintType type) const;
    std::vector<std::string> getRegisteredConstraintTypes() const;
    
    // 约束操作管理
    void registerConstraintOperation(const std::string& name, ConstraintNode::ConstraintOperation operation);
    void unregisterConstraintOperation(const std::string& name);
    ConstraintNode::ConstraintOperation getConstraintOperation(const std::string& name) const;
    std::string getConstraintOperationName(ConstraintNode::ConstraintOperation operation) const;
    std::vector<std::string> getRegisteredConstraintOperations() const;
    
    // 约束目标管理
    void addElementConstraint(const std::string& element, std::shared_ptr<ConstraintNode> constraint);
    void removeElementConstraint(const std::string& element, std::shared_ptr<ConstraintNode> constraint);
    std::vector<std::shared_ptr<ConstraintNode>> getElementConstraints(const std::string& element) const;
    
    void addPropertyConstraint(const std::string& property, std::shared_ptr<ConstraintNode> constraint);
    void removePropertyConstraint(const std::string& property, std::shared_ptr<ConstraintNode> constraint);
    std::vector<std::shared_ptr<ConstraintNode>> getPropertyConstraints(const std::string& property) const;
    
    void addTypeConstraint(const std::string& type, std::shared_ptr<ConstraintNode> constraint);
    void removeTypeConstraint(const std::string& type, std::shared_ptr<ConstraintNode> constraint);
    std::vector<std::shared_ptr<ConstraintNode>> getTypeConstraints(const std::string& type) const;
    
    // 约束条件管理
    void registerConditionFunction(const std::string& name, std::function<bool(const std::string&)> function);
    void unregisterConditionFunction(const std::string& name);
    std::function<bool(const std::string&)> getConditionFunction(const std::string& name) const;
    std::vector<std::string> getRegisteredConditionFunctions() const;
    
    void setConditionValue(const std::string& name, const std::string& value);
    std::string getConditionValue(const std::string& name) const;
    bool hasConditionValue(const std::string& name) const;
    void removeConditionValue(const std::string& name);
    
    // 约束处理
    bool processConstraints();
    bool processConstraint(std::shared_ptr<ConstraintNode> constraint);
    bool processConstraintGroup(const std::string& groupName);
    bool processConstraintType(ConstraintNode::ConstraintType type);
    bool processConstraintOperation(ConstraintNode::ConstraintOperation operation);
    
    // 约束验证
    bool validateConstraints() const;
    bool validateConstraint(std::shared_ptr<ConstraintNode> constraint) const;
    bool validateConstraintGroup(const std::string& groupName) const;
    bool validateConstraintType(ConstraintNode::ConstraintType type) const;
    bool validateConstraintOperation(ConstraintNode::ConstraintOperation operation) const;
    
    // 约束检查
    bool checkConstraints(const std::string& value) const;
    bool checkElementConstraints(const std::string& element, const std::string& value) const;
    bool checkPropertyConstraints(const std::string& property, const std::string& value) const;
    bool checkTypeConstraints(const std::string& type, const std::string& value) const;
    bool checkGlobalConstraints(const std::string& value) const;
    bool checkLocalConstraints(const std::string& value) const;
    bool checkInheritedConstraints(const std::string& value) const;
    
    // 约束应用
    bool applyConstraints(std::shared_ptr<BaseNode> node);
    bool applyElementConstraints(std::shared_ptr<ElementNode> element);
    bool applyPropertyConstraints(std::shared_ptr<ElementNode> element, const std::string& property, const std::string& value);
    bool applyTypeConstraints(std::shared_ptr<BaseNode> node, const std::string& type);
    
    // 约束解析
    std::shared_ptr<ConstraintNode> parseConstraint(const std::string& constraintText);
    std::vector<std::shared_ptr<ConstraintNode>> parseConstraints(const std::string& constraintsText);
    std::string parseConstraintExpression(const std::string& expression);
    std::vector<std::string> parseConstraintTargets(const std::string& targets);
    std::vector<std::string> parseConstraintConditions(const std::string& conditions);
    
    // 约束生成
    std::string generateConstraints() const;
    std::string generateConstraint(std::shared_ptr<ConstraintNode> constraint) const;
    std::string generateConstraintGroup(const std::string& groupName) const;
    std::string generateConstraintType(ConstraintNode::ConstraintType type) const;
    std::string generateConstraintOperation(ConstraintNode::ConstraintOperation operation) const;
    
    // 约束转换
    std::string toHTML() const;
    std::string toCSS() const;
    std::string toJavaScript() const;
    std::string toConstraintString() const;
    
    // 约束格式化
    std::string formatConstraints() const;
    std::string minifyConstraints() const;
    std::string beautifyConstraints() const;
    
    // 约束压缩
    std::string compressConstraints() const;
    std::string decompressConstraints() const;
    
    // 约束编码
    std::string encodeConstraints() const;
    std::string decodeConstraints() const;
    
    // 约束统计
    int getTotalConstraints() const { return totalConstraints; }
    int getActiveConstraints() const { return activeConstraints; }
    int getProcessedConstraints() const { return processedConstraints; }
    int getViolatedConstraints() const { return violatedConstraints; }
    int getErrorConstraints() const { return errorConstraints; }
    int getWarningConstraints() const { return warningConstraints; }
    
    // 约束配置
    void setConstraintConfig(const std::string& key, const std::string& value);
    std::string getConstraintConfig(const std::string& key) const;
    bool hasConstraintConfig(const std::string& key) const;
    void removeConstraintConfig(const std::string& key);
    
    void setConstraintFlag(const std::string& key, bool value);
    bool getConstraintFlag(const std::string& key) const;
    bool hasConstraintFlag(const std::string& key) const;
    void removeConstraintFlag(const std::string& key);
    
    void setConstraintLimit(const std::string& key, int value);
    int getConstraintLimit(const std::string& key) const;
    bool hasConstraintLimit(const std::string& key) const;
    void removeConstraintLimit(const std::string& key);
    
    // 约束优先级
    void setConstraintPriority(const std::string& name, int priority);
    int getConstraintPriority(const std::string& name) const;
    bool hasConstraintPriority(const std::string& name) const;
    void removeConstraintPriority(const std::string& name);
    std::vector<std::shared_ptr<ConstraintNode>> getConstraintsByPriority(int priority) const;
    std::vector<int> getConstraintPriorities() const;
    
    // 约束错误和警告
    const std::vector<std::string>& getValidationErrors() const { return validationErrors; }
    const std::vector<std::string>& getValidationWarnings() const { return validationWarnings; }
    const std::vector<std::string>& getConstraintViolations() const { return constraintViolations; }
    void addValidationError(const std::string& error);
    void addValidationWarning(const std::string& warning);
    void addConstraintViolation(const std::string& violation);
    void clearValidationMessages();
    void clearConstraintViolations();
    
    // 约束重置
    void reset();
    void resetConstraints();
    void resetConstraintGroups();
    void resetConstraintTypes();
    void resetConstraintOperations();
    void resetConstraintTargets();
    void resetConstraintConditions();
    void resetConstraintConfig();
    void resetConstraintPriorities();
    
    // 约束清理
    void clearConstraints();
    void clearConstraintGroups();
    void clearConstraintTypes();
    void clearConstraintOperations();
    void clearConstraintTargets();
    void clearConstraintConditions();
    void clearConstraintConfig();
    void clearConstraintPriorities();
    
    // 约束优化
    void optimizeConstraints();
    void optimizeConstraintGroups();
    void optimizeConstraintTypes();
    void optimizeConstraintOperations();
    void optimizeConstraintTargets();
    void optimizeConstraintConditions();
    
    // 约束调试
    std::string getDebugInfo() const;
    std::string getConstraintDebugInfo(std::shared_ptr<ConstraintNode> constraint) const;
    std::string getConstraintGroupDebugInfo(const std::string& groupName) const;
    std::string getConstraintTypeDebugInfo(ConstraintNode::ConstraintType type) const;
    std::string getConstraintOperationDebugInfo(ConstraintNode::ConstraintOperation operation) const;
    
    // 约束导出
    std::string exportConstraints() const;
    std::string exportConstraintGroup(const std::string& groupName) const;
    std::string exportConstraintType(ConstraintNode::ConstraintType type) const;
    std::string exportConstraintOperation(ConstraintNode::ConstraintOperation operation) const;
    
    // 约束导入
    bool importConstraints(const std::string& constraintsText);
    bool importConstraintGroup(const std::string& groupName, const std::string& groupText);
    bool importConstraintType(ConstraintNode::ConstraintType type, const std::string& typeText);
    bool importConstraintOperation(ConstraintNode::ConstraintOperation operation, const std::string& operationText);
    
    // 约束克隆
    std::shared_ptr<CHTLConstraintProcessor> clone() const;
    
    // 约束比较
    bool operator==(const CHTLConstraintProcessor& other) const;
    bool operator!=(const CHTLConstraintProcessor& other) const;

private:
    // 辅助方法
    void initializeConstraintTypes();
    void initializeConstraintOperations();
    void initializeConditionFunctions();
    void updateConstraintStatistics();
    void updateConstraintMappings();
    void updateConstraintPriorities();
    
    // 约束处理
    bool processExactConstraints();
    bool processTypeConstraints();
    bool processGlobalConstraints();
    bool processLocalConstraints();
    bool processInheritedConstraints();
    
    // 约束验证
    bool validateExactConstraints() const;
    bool validateTypeConstraints() const;
    bool validateGlobalConstraints() const;
    bool validateLocalConstraints() const;
    bool validateInheritedConstraints() const;
    
    // 约束检查
    bool checkExactConstraints(const std::string& value) const;
    
    // 约束应用
    bool applyExactConstraints(std::shared_ptr<BaseNode> node);
    bool applyGlobalConstraints(std::shared_ptr<BaseNode> node);
    bool applyLocalConstraints(std::shared_ptr<BaseNode> node);
    bool applyInheritedConstraints(std::shared_ptr<BaseNode> node);
    
    // 约束解析
    std::shared_ptr<ConstraintNode> parseExactConstraint(const std::string& constraintText);
    std::shared_ptr<ConstraintNode> parseTypeConstraint(const std::string& constraintText);
    std::shared_ptr<ConstraintNode> parseGlobalConstraint(const std::string& constraintText);
    std::shared_ptr<ConstraintNode> parseLocalConstraint(const std::string& constraintText);
    std::shared_ptr<ConstraintNode> parseInheritedConstraint(const std::string& constraintText);
    
    // 约束生成
    std::string generateExactConstraints() const;
    std::string generateTypeConstraints() const;
    std::string generateGlobalConstraints() const;
    std::string generateLocalConstraints() const;
    std::string generateInheritedConstraints() const;
    
    // 错误处理
    void handleConstraintError(const std::string& error);
    void handleConstraintWarning(const std::string& warning);
    void handleConstraintViolation(const std::string& violation);
    
    // 约束优化
    void optimizeConstraint(std::shared_ptr<ConstraintNode> constraint);
    void optimizeConstraintGroup(const std::string& groupName);
    void optimizeConstraintType(ConstraintNode::ConstraintType type);
    void optimizeConstraintOperation(ConstraintNode::ConstraintOperation operation);
    
    // 约束调试
    std::string formatConstraintDebugInfo(std::shared_ptr<ConstraintNode> constraint) const;
    std::string formatConstraintGroupDebugInfo(const std::string& groupName) const;
    std::string formatConstraintTypeDebugInfo(ConstraintNode::ConstraintType type) const;
    std::string formatConstraintOperationDebugInfo(ConstraintNode::ConstraintOperation operation) const;
};

} // namespace CHTL

#endif // CHTL_CONSTRAINT_PROCESSOR_H