#ifndef CHTL_USE_PROCESSOR_H
#define CHTL_USE_PROCESSOR_H

#include "../CHTLNode/UseNode.h"
#include "../CHTLNode/BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

class CHTLUseProcessor {
private:
    // Use处理器状态
    bool debugMode;
    bool strictMode;
    bool enableUses;
    bool enableValidation;
    bool enableWarnings;
    bool enableErrors;
    
    // Use存储
    std::vector<std::shared_ptr<UseNode>> uses;
    std::map<std::string, std::shared_ptr<UseNode>> useMap;
    std::map<std::string, std::vector<std::shared_ptr<UseNode>>> useGroups;
    
    // Use类型映射
    std::map<std::string, UseNode::UseType> useTypeMap;
    std::map<UseNode::UseType, std::string> useTypeNames;
    
    // Use规则映射
    std::map<std::string, std::function<bool(const std::string&)>> ruleFunctions;
    std::map<std::string, std::string> ruleValues;
    
    // Use验证
    std::vector<std::string> validationErrors;
    std::vector<std::string> validationWarnings;
    std::vector<std::string> useErrors;
    
    // Use统计
    int totalUses;
    int activeUses;
    int processedUses;
    int validUses;
    int errorUses;
    int warningUses;
    
    // Use设置
    std::map<std::string, std::string> useSettings;
    std::map<std::string, bool> useFlags;
    std::map<std::string, int> useLimits;
    std::map<std::string, std::string> useDefaults;
    
    // Use优先级
    std::map<std::string, int> usePriorities;
    std::map<int, std::vector<std::shared_ptr<UseNode>>> priorityUses;

public:
    CHTLUseProcessor();
    virtual ~CHTLUseProcessor() = default;
    
    // 配置方法
    void setDebugMode(bool debug) { debugMode = debug; }
    bool isDebugMode() const { return debugMode; }
    
    void setStrictMode(bool strict) { strictMode = strict; }
    bool isStrictMode() const { return strictMode; }
    
    void setEnableUses(bool enable) { enableUses = enable; }
    bool isUsesEnabled() const { return enableUses; }
    
    void setEnableValidation(bool enable) { enableValidation = enable; }
    bool isValidationEnabled() const { return enableValidation; }
    
    void setEnableWarnings(bool enable) { enableWarnings = enable; }
    bool isWarningsEnabled() const { return enableWarnings; }
    
    void setEnableErrors(bool enable) { enableErrors = enable; }
    bool isErrorsEnabled() const { return enableErrors; }
    
    // Use管理
    void addUse(std::shared_ptr<UseNode> use);
    void removeUse(const std::string& name);
    void removeUse(std::shared_ptr<UseNode> use);
    std::shared_ptr<UseNode> getUse(const std::string& name) const;
    std::vector<std::shared_ptr<UseNode>> getUses() const;
    std::vector<std::shared_ptr<UseNode>> getUsesByType(UseNode::UseType type) const;
    std::vector<std::shared_ptr<UseNode>> getUsesByGroup(const std::string& group) const;
    
    // Use组管理
    void addUseToGroup(const std::string& groupName, std::shared_ptr<UseNode> use);
    void removeUseFromGroup(const std::string& groupName, std::shared_ptr<UseNode> use);
    std::vector<std::string> getUseGroups() const;
    
    // Use类型管理
    void registerUseType(const std::string& name, UseNode::UseType type);
    void unregisterUseType(const std::string& name);
    UseNode::UseType getUseType(const std::string& name) const;
    std::string getUseTypeName(UseNode::UseType type) const;
    std::vector<std::string> getRegisteredUseTypes() const;
    
    // Use规则管理
    void registerRuleFunction(const std::string& name, std::function<bool(const std::string&)> function);
    void unregisterRuleFunction(const std::string& name);
    std::function<bool(const std::string&)> getRuleFunction(const std::string& name) const;
    std::vector<std::string> getRegisteredRuleFunctions() const;
    
    void setRuleValue(const std::string& name, const std::string& value);
    std::string getRuleValue(const std::string& name) const;
    bool hasRuleValue(const std::string& name) const;
    void removeRuleValue(const std::string& name);
    
    // Use处理
    bool processUses();
    bool processUse(std::shared_ptr<UseNode> use);
    bool processUseGroup(const std::string& groupName);
    bool processUseType(UseNode::UseType type);
    
    // Use验证
    bool validateUses() const;
    bool validateUse(std::shared_ptr<UseNode> use) const;
    bool validateUseGroup(const std::string& groupName) const;
    bool validateUseType(UseNode::UseType type) const;
    
    // Use应用
    bool applyUses();
    bool applyUse(std::shared_ptr<UseNode> use);
    bool applyUseGroup(const std::string& groupName);
    bool applyUseType(UseNode::UseType type);
    
    // Use解析
    std::shared_ptr<UseNode> parseUse(const std::string& useText);
    std::vector<std::shared_ptr<UseNode>> parseUses(const std::string& usesText);
    std::string parseUseParameter(const std::string& parameter);
    std::vector<std::string> parseUseArray(const std::string& array);
    std::map<std::string, std::string> parseUseObject(const std::string& object);
    
    // Use生成
    std::string generateUses() const;
    std::string generateUse(std::shared_ptr<UseNode> use) const;
    std::string generateUseGroup(const std::string& groupName) const;
    std::string generateUseType(UseNode::UseType type) const;
    
    // Use转换
    std::string toHTML() const;
    std::string toCSS() const;
    std::string toJavaScript() const;
    std::string toUseString() const;
    
    // Use格式化
    std::string formatUses() const;
    std::string minifyUses() const;
    std::string beautifyUses() const;
    
    // Use压缩
    std::string compressUses() const;
    std::string decompressUses() const;
    
    // Use编码
    std::string encodeUses() const;
    std::string decodeUses() const;
    
    // Use统计
    int getTotalUses() const { return totalUses; }
    int getActiveUses() const { return activeUses; }
    int getProcessedUses() const { return processedUses; }
    int getValidUses() const { return validUses; }
    int getErrorUses() const { return errorUses; }
    int getWarningUses() const { return warningUses; }
    
    // Use设置
    void setUseSetting(const std::string& key, const std::string& value);
    std::string getUseSetting(const std::string& key) const;
    bool hasUseSetting(const std::string& key) const;
    void removeUseSetting(const std::string& key);
    
    void setUseFlag(const std::string& key, bool value);
    bool getUseFlag(const std::string& key) const;
    bool hasUseFlag(const std::string& key) const;
    void removeUseFlag(const std::string& key);
    
    void setUseLimit(const std::string& key, int value);
    int getUseLimit(const std::string& key) const;
    bool hasUseLimit(const std::string& key) const;
    void removeUseLimit(const std::string& key);
    
    void setUseDefault(const std::string& key, const std::string& value);
    std::string getUseDefault(const std::string& key) const;
    bool hasUseDefault(const std::string& key) const;
    void removeUseDefault(const std::string& key);
    
    // Use优先级
    void setUsePriority(const std::string& name, int priority);
    int getUsePriority(const std::string& name) const;
    bool hasUsePriority(const std::string& name) const;
    void removeUsePriority(const std::string& name);
    std::vector<std::shared_ptr<UseNode>> getUsesByPriority(int priority) const;
    std::vector<int> getUsePriorities() const;
    
    // Use错误和警告
    const std::vector<std::string>& getValidationErrors() const { return validationErrors; }
    const std::vector<std::string>& getValidationWarnings() const { return validationWarnings; }
    const std::vector<std::string>& getUseErrors() const { return useErrors; }
    void addValidationError(const std::string& error);
    void addValidationWarning(const std::string& warning);
    void addUseError(const std::string& error);
    void clearValidationMessages();
    void clearUseErrors();
    
    // Use重置
    void reset();
    void resetUses();
    void resetUseGroups();
    void resetUseTypes();
    void resetUseRules();
    void resetUseSettings();
    void resetUsePriorities();
    
    // Use清理
    void clearUses();
    void clearUseGroups();
    void clearUseTypes();
    void clearUseRules();
    void clearUseSettings();
    void clearUsePriorities();
    
    // Use优化
    void optimizeUses();
    void optimizeUseGroups();
    void optimizeUseTypes();
    void optimizeUseRules();
    
    // Use调试
    std::string getDebugInfo() const;
    std::string getUseDebugInfo(std::shared_ptr<UseNode> use) const;
    std::string getUseGroupDebugInfo(const std::string& groupName) const;
    std::string getUseTypeDebugInfo(UseNode::UseType type) const;
    
    // Use导出
    std::string exportUses() const;
    std::string exportUseGroup(const std::string& groupName) const;
    std::string exportUseType(UseNode::UseType type) const;
    
    // Use导入
    bool importUses(const std::string& usesText);
    bool importUseGroup(const std::string& groupName, const std::string& groupText);
    bool importUseType(UseNode::UseType type, const std::string& typeText);
    
    // Use克隆
    std::shared_ptr<CHTLUseProcessor> clone() const;
    
    // Use比较
    bool operator==(const CHTLUseProcessor& other) const;
    bool operator!=(const CHTLUseProcessor& other) const;

private:
    // 辅助方法
    void initializeUseTypes();
    void initializeRuleFunctions();
    void updateUseStatistics();
    void updateUseMappings();
    void updateUsePriorities();
    
    // Use处理
    bool processHTML5Uses();
    bool processConfigurationUses();
    bool processModuleUses();
    bool processTemplateUses();
    bool processCustomUses();
    
    // Use验证
    bool validateHTML5Uses() const;
    bool validateConfigurationUses() const;
    bool validateModuleUses() const;
    bool validateTemplateUses() const;
    bool validateCustomUses() const;
    
    // Use应用
    bool applyHTML5Uses();
    bool applyConfigurationUses();
    bool applyModuleUses();
    bool applyTemplateUses();
    bool applyCustomUses();
    
    // Use解析
    std::shared_ptr<UseNode> parseHTML5Use(const std::string& useText);
    std::shared_ptr<UseNode> parseConfigurationUse(const std::string& useText);
    std::shared_ptr<UseNode> parseModuleUse(const std::string& useText);
    std::shared_ptr<UseNode> parseTemplateUse(const std::string& useText);
    std::shared_ptr<UseNode> parseCustomUse(const std::string& useText);
    
    // Use生成
    std::string generateHTML5Uses() const;
    std::string generateConfigurationUses() const;
    std::string generateModuleUses() const;
    std::string generateTemplateUses() const;
    std::string generateCustomUses() const;
    
    // 错误处理
    void handleUseError(const std::string& error);
    void handleUseWarning(const std::string& warning);
    
    // Use优化
    void optimizeUse(std::shared_ptr<UseNode> use);
    void optimizeUseGroup(const std::string& groupName);
    void optimizeUseType(UseNode::UseType type);
    void optimizeUseRule(const std::string& rule);
    
    // Use调试
    std::string formatUseDebugInfo(std::shared_ptr<UseNode> use) const;
    std::string formatUseGroupDebugInfo(const std::string& groupName) const;
    std::string formatUseTypeDebugInfo(UseNode::UseType type) const;
};

} // namespace CHTL

#endif // CHTL_USE_PROCESSOR_H