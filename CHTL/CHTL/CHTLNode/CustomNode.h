#pragma once

#include "BaseNode.h"

namespace CHTL {

enum class CustomType {
    STYLE,      // 自定义样式组
    ELEMENT,    // 自定义元素
    VAR         // 自定义变量组
};

class CustomNode : public BaseNode {
private:
    CustomType customType;
    std::string customName;
    std::map<std::string, std::string> variables;
    std::vector<std::shared_ptr<BaseNode>> children;
    
    // 特例化操作
    std::vector<std::string> deletedProperties;
    std::vector<std::string> deletedInheritances;
    std::map<std::string, std::string> overriddenProperties;
    
public:
    CustomNode(CustomType type, const std::string& name);
    virtual ~CustomNode() = default;
    
    // 自定义类型
    CustomType getCustomType() const { return customType; }
    void setCustomType(CustomType type) { customType = type; }
    
    // 自定义名称
    const std::string& getCustomName() const { return customName; }
    void setCustomName(const std::string& name) { customName = name; }
    
    // 变量管理
    void setVariable(const std::string& key, const std::string& value);
    std::string getVariable(const std::string& key) const;
    bool hasVariable(const std::string& key) const;
    void removeVariable(const std::string& key);
    const std::map<std::string, std::string>& getVariables() const { return variables; }
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    std::vector<std::shared_ptr<BaseNode>>& getChildren() { return children; }
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children; }
    
    // 特例化操作
    void deleteProperty(const std::string& property);
    void deleteInheritance(const std::string& inheritance);
    void overrideProperty(const std::string& property, const std::string& value);
    
    const std::vector<std::string>& getDeletedProperties() const { return deletedProperties; }
    const std::vector<std::string>& getDeletedInheritances() const { return deletedInheritances; }
    const std::map<std::string, std::string>& getOverriddenProperties() const { return overriddenProperties; }
    
    // 继承管理
    void addInheritance(const std::string& customName);
    std::vector<std::string> getInheritances() const;
    
    // 索引访问
    std::shared_ptr<BaseNode> getChildByIndex(size_t index) const;
    void insertChildAt(size_t index, std::shared_ptr<BaseNode> child);
    void removeChildAt(size_t index);
    
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
    
private:
    std::vector<std::string> inheritances;
};

} // namespace CHTL