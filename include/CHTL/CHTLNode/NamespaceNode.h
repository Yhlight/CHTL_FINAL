#pragma once

#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 命名空间节点
 * 表示CHTL命名空间定义
 */
class NamespaceNode : public BaseNode {
public:
    NamespaceNode(const std::string& name);
    virtual ~NamespaceNode();
    
    // 命名空间名称
    void setNamespaceName(const std::string& name);
    std::string getNamespaceName() const;
    
    // 嵌套命名空间
    void addNestedNamespace(const std::string& name, std::shared_ptr<NamespaceNode> namespaceNode);
    std::shared_ptr<NamespaceNode> getNestedNamespace(const std::string& name) const;
    bool hasNestedNamespace(const std::string& name) const;
    const std::unordered_map<std::string, std::shared_ptr<NamespaceNode>>& getNestedNamespaces() const;
    
    // 模板定义
    void addTemplate(const std::string& name, std::shared_ptr<class TemplateNode> templateNode);
    std::shared_ptr<class TemplateNode> getTemplate(const std::string& name) const;
    bool hasTemplate(const std::string& name) const;
    const std::unordered_map<std::string, std::shared_ptr<class TemplateNode>>& getTemplates() const;
    
    // 自定义定义
    void addCustom(const std::string& name, std::shared_ptr<class CustomNode> customNode);
    std::shared_ptr<class CustomNode> getCustom(const std::string& name) const;
    bool hasCustom(const std::string& name) const;
    const std::unordered_map<std::string, std::shared_ptr<class CustomNode>>& getCustoms() const;
    
    // 原始嵌入定义
    void addOrigin(const std::string& name, std::shared_ptr<class OriginNode> originNode);
    std::shared_ptr<class OriginNode> getOrigin(const std::string& name) const;
    bool hasOrigin(const std::string& name) const;
    const std::unordered_map<std::string, std::shared_ptr<class OriginNode>>& getOrigins() const;
    
    // 配置定义
    void addConfig(const std::string& name, std::shared_ptr<class ConfigNode> configNode);
    std::shared_ptr<class ConfigNode> getConfig(const std::string& name) const;
    bool hasConfig(const std::string& name) const;
    const std::unordered_map<std::string, std::shared_ptr<class ConfigNode>>& getConfigs() const;
    
    // 约束定义
    void addConstraint(const std::string& type, const std::string& target);
    std::vector<std::pair<std::string, std::string>> getConstraints() const;
    bool hasConstraint(const std::string& type, const std::string& target) const;
    
    // 全局约束
    void addGlobalConstraint(const std::string& type);
    std::vector<std::string> getGlobalConstraints() const;
    bool hasGlobalConstraint(const std::string& type) const;
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) override;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 节点验证
    virtual bool validate() const override;
    
    // 调试信息
    virtual std::string toString() const override;

private:
    std::string m_namespaceName;
    std::unordered_map<std::string, std::shared_ptr<NamespaceNode>> m_nestedNamespaces;
    std::unordered_map<std::string, std::shared_ptr<class TemplateNode>> m_templates;
    std::unordered_map<std::string, std::shared_ptr<class CustomNode>> m_customs;
    std::unordered_map<std::string, std::shared_ptr<class OriginNode>> m_origins;
    std::unordered_map<std::string, std::shared_ptr<class ConfigNode>> m_configs;
    std::vector<std::pair<std::string, std::string>> m_constraints;
    std::vector<std::string> m_globalConstraints;
};

} // namespace CHTL