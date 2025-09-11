#ifndef CHTL_USE_NODE_H
#define CHTL_USE_NODE_H

#include "BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

class UseNode : public BaseNode {
public:
    // Use类型
    enum class UseType {
        HTML5,          // HTML5类型
        CONFIGURATION,  // 配置组类型
        MODULE,         // 模块类型
        TEMPLATE,       // 模板类型
        CUSTOM          // 自定义类型
    };

private:
    UseType useType;
    std::string useName;
    std::string useGroup;
    std::string useVersion;
    
    // Use参数
    std::map<std::string, std::string> useParameters;
    std::map<std::string, std::vector<std::string>> useArrays;
    std::map<std::string, std::map<std::string, std::string>> useObjects;
    
    // Use选项
    std::map<std::string, bool> useOptions;
    std::map<std::string, int> useLimits;
    std::map<std::string, std::string> useDefaults;
    
    // Use规则
    std::vector<std::string> useRules;
    std::map<std::string, std::string> ruleValues;
    std::map<std::string, std::function<bool(const std::string&)>> ruleFunctions;
    
    // Use状态
    bool isActive;
    bool isProcessed;
    bool isValid;
    std::vector<std::string> useErrors;
    std::vector<std::string> useWarnings;
    
    // Use元数据
    std::map<std::string, std::string> metadata;
    int priority;
    bool isRequired;
    bool isOptional;
    bool isInherited;

public:
    UseNode(const std::string& name, UseType type = UseType::HTML5);
    virtual ~UseNode() = default;
    
    // Use类型
    UseType getUseType() const { return useType; }
    void setUseType(UseType type) { useType = type; }
    std::string getUseTypeName() const;
    
    // Use名称
    const std::string& getUseName() const { return useName; }
    void setUseName(const std::string& name) { useName = name; }
    
    // Use组
    const std::string& getUseGroup() const { return useGroup; }
    void setUseGroup(const std::string& group) { useGroup = group; }
    
    // Use版本
    const std::string& getUseVersion() const { return useVersion; }
    void setUseVersion(const std::string& version) { useVersion = version; }
    
    // Use参数
    void setUseParameter(const std::string& key, const std::string& value);
    std::string getUseParameter(const std::string& key) const;
    bool hasUseParameter(const std::string& key) const;
    void removeUseParameter(const std::string& key);
    const std::map<std::string, std::string>& getUseParameters() const { return useParameters; }
    
    // Use数组
    void setUseArray(const std::string& key, const std::vector<std::string>& values);
    std::vector<std::string> getUseArray(const std::string& key) const;
    bool hasUseArray(const std::string& key) const;
    void removeUseArray(const std::string& key);
    void addUseArrayItem(const std::string& key, const std::string& item);
    void removeUseArrayItem(const std::string& key, const std::string& item);
    const std::map<std::string, std::vector<std::string>>& getUseArrays() const { return useArrays; }
    
    // Use对象
    void setUseObject(const std::string& key, const std::map<std::string, std::string>& object);
    std::map<std::string, std::string> getUseObject(const std::string& key) const;
    bool hasUseObject(const std::string& key) const;
    void removeUseObject(const std::string& key);
    void setUseObjectProperty(const std::string& key, const std::string& property, const std::string& value);
    std::string getUseObjectProperty(const std::string& key, const std::string& property) const;
    void removeUseObjectProperty(const std::string& key, const std::string& property);
    const std::map<std::string, std::map<std::string, std::string>>& getUseObjects() const { return useObjects; }
    
    // Use选项
    void setUseOption(const std::string& key, bool value);
    bool getUseOption(const std::string& key) const;
    bool hasUseOption(const std::string& key) const;
    void removeUseOption(const std::string& key);
    const std::map<std::string, bool>& getUseOptions() const { return useOptions; }
    
    // Use限制
    void setUseLimit(const std::string& key, int value);
    int getUseLimit(const std::string& key) const;
    bool hasUseLimit(const std::string& key) const;
    void removeUseLimit(const std::string& key);
    const std::map<std::string, int>& getUseLimits() const { return useLimits; }
    
    // Use默认值
    void setUseDefault(const std::string& key, const std::string& value);
    std::string getUseDefault(const std::string& key) const;
    bool hasUseDefault(const std::string& key) const;
    void removeUseDefault(const std::string& key);
    const std::map<std::string, std::string>& getUseDefaults() const { return useDefaults; }
    
    // Use规则
    void addUseRule(const std::string& rule);
    void removeUseRule(const std::string& rule);
    const std::vector<std::string>& getUseRules() const { return useRules; }
    bool hasUseRule(const std::string& rule) const;
    
    void setRuleValue(const std::string& rule, const std::string& value);
    std::string getRuleValue(const std::string& rule) const;
    bool hasRuleValue(const std::string& rule) const;
    void removeRuleValue(const std::string& rule);
    const std::map<std::string, std::string>& getRuleValues() const { return ruleValues; }
    
    void setRuleFunction(const std::string& rule, std::function<bool(const std::string&)> function);
    std::function<bool(const std::string&)> getRuleFunction(const std::string& rule) const;
    bool hasRuleFunction(const std::string& rule) const;
    void removeRuleFunction(const std::string& rule);
    
    // Use状态
    bool isActiveUse() const { return isActive; }
    void setActive(bool active) { isActive = active; }
    
    bool isProcessedUse() const { return isProcessed; }
    void setProcessed(bool processed) { isProcessed = processed; }
    
    bool isValidUse() const { return isValid; }
    void setValid(bool valid) { isValid = valid; }
    
    // Use错误和警告
    const std::vector<std::string>& getUseErrors() const { return useErrors; }
    const std::vector<std::string>& getUseWarnings() const { return useWarnings; }
    void addUseError(const std::string& error);
    void addUseWarning(const std::string& warning);
    void clearUseMessages();
    
    // Use元数据
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    const std::map<std::string, std::string>& getMetadata() const { return metadata; }
    
    // Use特性
    int getPriority() const { return priority; }
    void setPriority(int pri) { priority = pri; }
    
    bool isRequiredUse() const { return isRequired; }
    void setRequired(bool required) { isRequired = required; }
    
    bool isOptionalUse() const { return isOptional; }
    void setOptional(bool optional) { isOptional = optional; }
    
    bool isInheritedUse() const { return isInherited; }
    void setInherited(bool inherited) { isInherited = inherited; }
    
    // Use处理
    bool processUse();
    bool processHTML5Use();
    bool processConfigurationUse();
    bool processModuleUse();
    bool processTemplateUse();
    bool processCustomUse();
    
    // Use验证
    bool validateUse() const;
    bool validateParameters() const;
    bool validateOptions() const;
    bool validateLimits() const;
    bool validateDefaults() const;
    bool validateRules() const;
    
    // Use应用
    bool applyUse();
    bool applyUseRules();
    bool applyUseOptions();
    bool applyUseLimits();
    bool applyUseDefaults();
    
    // Use转换
    std::string toHTML() const override;
    std::string toCSS() const;
    std::string toJavaScript() const;
    std::string toUseString() const;
    
    // Use格式化
    std::string formatUse() const;
    std::string minifyUse() const;
    std::string beautifyUse() const;
    
    // Use压缩
    std::string compressUse() const;
    std::string decompressUse() const;
    
    // Use编码
    std::string encodeUse() const;
    std::string decodeUse() const;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const UseNode& other) const;
    bool operator!=(const UseNode& other) const;

private:
    // 辅助方法
    std::string processUseParameter(const std::string& key, const std::string& value) const;
    std::string validateUseParameter(const std::string& key, const std::string& value) const;
    std::string sanitizeUseParameter(const std::string& key, const std::string& value) const;
    
    // 规则处理
    std::string parseUseRule(const std::string& rule) const;
    std::string evaluateUseRule(const std::string& rule) const;
    std::string optimizeUseRule(const std::string& rule) const;
    
    // Use处理
    void processUseParameters();
    void processUseOptions();
    void processUseLimits();
    void processUseDefaults();
    void processUseRules();
    
    // Use验证
    bool validateUseParameters() const;
    bool validateUseOptions() const;
    bool validateUseLimits() const;
    bool validateUseDefaults() const;
    bool validateUseRules() const;
    
    // Use应用
    void applyUseParameters();
    
    // 错误处理
    void handleUseError(const std::string& error);
    void handleUseWarning(const std::string& warning);
};

} // namespace CHTL

#endif // CHTL_USE_NODE_H