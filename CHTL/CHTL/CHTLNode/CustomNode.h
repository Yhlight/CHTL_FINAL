#ifndef CHTL_CUSTOM_NODE_H
#define CHTL_CUSTOM_NODE_H

#include "BaseNode.h"
#include "TemplateNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

class CustomNode : public BaseNode {
private:
    // 自定义类型
    enum class CustomType {
        STYLE,      // 自定义样式组
        ELEMENT,    // 自定义元素
        VARIABLE,   // 自定义变量
        FUNCTION,   // 自定义函数
        MIXED       // 混合自定义
    };
    
    CustomType customType;
    std::string customName;
    std::map<std::string, std::string> parameters;
    std::vector<std::shared_ptr<BaseNode>> customContent;
    
    // 特例化操作
    std::map<std::string, std::shared_ptr<BaseNode>> specializations;
    std::vector<std::string> specializationKeys;
    
    // 索引访问
    std::map<int, std::shared_ptr<BaseNode>> indexedContent;
    std::vector<int> indexKeys;
    
    // 插入删除操作
    std::vector<std::shared_ptr<BaseNode>> insertedElements;
    std::vector<std::shared_ptr<BaseNode>> deletedElements;
    
    // 自定义约束
    std::vector<std::string> constraints;
    std::map<std::string, std::string> typeConstraints;
    std::map<std::string, std::function<bool(const std::string&)>> validationFunctions;
    
    // 自定义元数据
    std::map<std::string, std::string> metadata;
    bool isAbstract;
    bool isFinal;
    int priority;
    
    // 自定义行为
    std::map<std::string, std::function<std::string(const std::vector<std::string>&)>> customMethods;
    std::map<std::string, std::function<std::shared_ptr<BaseNode>(const std::map<std::string, std::string>&)>> customConstructors;

public:
    CustomNode(const std::string& name, CustomType type = CustomType::ELEMENT);
    virtual ~CustomNode() = default;
    
    // 自定义类型
    CustomType getCustomType() const { return customType; }
    void setCustomType(CustomType type) { customType = type; }
    std::string getCustomTypeName() const;
    
    // 自定义名称
    const std::string& getCustomName() const { return customName; }
    void setCustomName(const std::string& name) { customName = name; }
    
    // 参数管理
    void setParameter(const std::string& name, const std::string& value);
    std::string getParameter(const std::string& name) const;
    bool hasParameter(const std::string& name) const;
    void removeParameter(const std::string& name);
    const std::map<std::string, std::string>& getParameters() const { return parameters; }
    
    // 自定义内容
    void addCustomContent(std::shared_ptr<BaseNode> content);
    void removeCustomContent(std::shared_ptr<BaseNode> content);
    std::vector<std::shared_ptr<BaseNode>>& getCustomContent() { return customContent; }
    const std::vector<std::shared_ptr<BaseNode>>& getCustomContent() const { return customContent; }
    
    // 特例化操作
    void addSpecialization(const std::string& key, std::shared_ptr<BaseNode> specialization);
    void removeSpecialization(const std::string& key);
    std::shared_ptr<BaseNode> getSpecialization(const std::string& key) const;
    bool hasSpecialization(const std::string& key) const;
    const std::map<std::string, std::shared_ptr<BaseNode>>& getSpecializations() const { return specializations; }
    const std::vector<std::string>& getSpecializationKeys() const { return specializationKeys; }
    
    // 索引访问
    void setIndexedContent(int index, std::shared_ptr<BaseNode> content);
    void removeIndexedContent(int index);
    std::shared_ptr<BaseNode> getIndexedContent(int index) const;
    bool hasIndexedContent(int index) const;
    const std::map<int, std::shared_ptr<BaseNode>>& getIndexedContent() const { return indexedContent; }
    const std::vector<int>& getIndexKeys() const { return indexKeys; }
    
    // 插入删除操作
    void insertElement(std::shared_ptr<BaseNode> element);
    void insertElementAt(int index, std::shared_ptr<BaseNode> element);
    void removeElement(std::shared_ptr<BaseNode> element);
    void removeElementAt(int index);
    void clearElements();
    const std::vector<std::shared_ptr<BaseNode>>& getInsertedElements() const { return insertedElements; }
    const std::vector<std::shared_ptr<BaseNode>>& getDeletedElements() const { return deletedElements; }
    
    // 自定义约束
    void addConstraint(const std::string& constraint);
    void removeConstraint(const std::string& constraint);
    const std::vector<std::string>& getConstraints() const { return constraints; }
    bool hasConstraint(const std::string& constraint) const;
    
    void setTypeConstraint(const std::string& type, const std::string& constraint);
    std::string getTypeConstraint(const std::string& type) const;
    bool hasTypeConstraint(const std::string& type) const;
    void removeTypeConstraint(const std::string& type);
    const std::map<std::string, std::string>& getTypeConstraints() const { return typeConstraints; }
    
    void setValidationFunction(const std::string& type, std::function<bool(const std::string&)> validator);
    std::function<bool(const std::string&)> getValidationFunction(const std::string& type) const;
    bool hasValidationFunction(const std::string& type) const;
    void removeValidationFunction(const std::string& type);
    
    // 自定义元数据
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    const std::map<std::string, std::string>& getMetadata() const { return metadata; }
    
    // 自定义特性
    bool isAbstractCustom() const { return isAbstract; }
    void setAbstract(bool abstract) { isAbstract = abstract; }
    
    bool isFinalCustom() const { return isFinal; }
    void setFinal(bool final) { isFinal = final; }
    
    int getPriority() const { return priority; }
    void setPriority(int pri) { priority = pri; }
    
    // 自定义方法
    void setCustomMethod(const std::string& name, std::function<std::string(const std::vector<std::string>&)> method);
    std::string callCustomMethod(const std::string& name, const std::vector<std::string>& args) const;
    bool hasCustomMethod(const std::string& name) const;
    void removeCustomMethod(const std::string& name);
    
    // 自定义构造函数
    void setCustomConstructor(const std::string& name, std::function<std::shared_ptr<BaseNode>(const std::map<std::string, std::string>&)> constructor);
    std::shared_ptr<BaseNode> callCustomConstructor(const std::string& name, const std::map<std::string, std::string>& args) const;
    bool hasCustomConstructor(const std::string& name) const;
    void removeCustomConstructor(const std::string& name);
    
    // 自定义实例化
    std::shared_ptr<BaseNode> instantiate(const std::map<std::string, std::string>& args = {}) const;
    std::shared_ptr<BaseNode> instantiateStyleCustom(const std::map<std::string, std::string>& args = {}) const;
    std::shared_ptr<BaseNode> instantiateElementCustom(const std::map<std::string, std::string>& args = {}) const;
    std::shared_ptr<BaseNode> instantiateVariableCustom(const std::map<std::string, std::string>& args = {}) const;
    std::shared_ptr<BaseNode> instantiateFunctionCustom(const std::map<std::string, std::string>& args = {}) const;
    
    // 自定义组合
    std::shared_ptr<CustomNode> combine(const std::shared_ptr<CustomNode>& other) const;
    std::shared_ptr<CustomNode> merge(const std::shared_ptr<CustomNode>& other) const;
    
    // 自定义验证
    bool validateCustom() const;
    bool validateParameters(const std::map<std::string, std::string>& args) const;
    bool validateConstraints() const;
    bool validateSpecializations() const;
    
    // 自定义转换
    std::string toHTML() const override;
    std::string toCSS() const;
    std::string toJavaScript() const;
    std::string toCustomString() const;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const CustomNode& other) const;
    bool operator!=(const CustomNode& other) const;

private:
    // 辅助方法
    std::string processCustomString(const std::string& customStr, const std::map<std::string, std::string>& args) const;
    std::string replacePlaceholders(const std::string& str, const std::map<std::string, std::string>& replacements) const;
    std::vector<std::string> parseCustomString(const std::string& customStr) const;
    
    // 自定义解析
    std::string parseStyleCustom(const std::string& customStr) const;
    std::string parseElementCustom(const std::string& customStr) const;
    std::string parseVariableCustom(const std::string& customStr) const;
    std::string parseFunctionCustom(const std::string& customStr) const;
    
    // 特例化处理
    void processSpecializations();
    void mergeSpecializations();
    void resolveSpecializationConflicts();
    
    // 约束检查
    bool checkTypeConstraint(const std::string& type, const std::string& value) const;
    bool checkGeneralConstraint(const std::string& constraint) const;
    bool checkValidationFunction(const std::string& type, const std::string& value) const;
    
    // 索引处理
    void updateIndexKeys();
    void sortIndexKeys();
    
    // 插入删除处理
    void processInsertions();
    void processDeletions();
    void updateElementLists();
};

} // namespace CHTL

#endif // CHTL_CUSTOM_NODE_H