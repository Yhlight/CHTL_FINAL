#pragma once

#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 自定义类型枚举
 */
enum class CustomType {
    STYLE,      // 自定义样式组
    ELEMENT,    // 自定义元素
    VAR         // 自定义变量组
};

/**
 * @brief 自定义节点
 * 表示CHTL自定义定义
 */
class CustomNode : public BaseNode {
public:
    CustomNode(CustomType type, const std::string& name);
    virtual ~CustomNode();
    
    // 自定义类型
    CustomType getCustomType() const;
    
    // 自定义名称
    void setCustomName(const std::string& name);
    std::string getCustomName() const;
    
    // 特例化操作
    void addSpecialization(const std::string& operation, const std::string& target);
    std::vector<std::pair<std::string, std::string>> getSpecializations() const;
    
    // 删除操作
    void addDeleteOperation(const std::string& target);
    std::vector<std::string> getDeleteOperations() const;
    
    // 插入操作
    void addInsertOperation(const std::string& position, const std::string& target, 
                          std::shared_ptr<BaseNode> content);
    std::vector<std::tuple<std::string, std::string, std::shared_ptr<BaseNode>>> getInsertOperations() const;
    
    // 无值属性（用于无值样式组）
    void addUnvaluedProperty(const std::string& property);
    std::vector<std::string> getUnvaluedProperties() const;
    bool hasUnvaluedProperty(const std::string& property) const;
    
    // 变量特例化（用于变量组）
    void addVariableSpecialization(const std::string& name, const std::string& value);
    std::string getVariableSpecialization(const std::string& name) const;
    bool hasVariableSpecialization(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& getVariableSpecializations() const;
    
    // 全缀名支持
    void setFullQualifiedName(const std::string& name);
    std::string getFullQualifiedName() const;
    bool hasFullQualifiedName() const;
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) override;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 节点验证
    virtual bool validate() const override;
    
    // 调试信息
    virtual std::string toString() const override;

private:
    CustomType m_customType;
    std::string m_customName;
    std::string m_fullQualifiedName;
    std::vector<std::pair<std::string, std::string>> m_specializations;
    std::vector<std::string> m_deleteOperations;
    std::vector<std::tuple<std::string, std::string, std::shared_ptr<BaseNode>>> m_insertOperations;
    std::vector<std::string> m_unvaluedProperties;
    std::unordered_map<std::string, std::string> m_variableSpecializations;
};

} // namespace CHTL