#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 命名空间节点类
 * 
 * 表示CHTL中的命名空间节点
 */
class NamespaceNode : public BaseNode {
public:
    NamespaceNode(const std::string& name);
    ~NamespaceNode() = default;

    /**
     * @brief 获取命名空间名称
     * @return 命名空间名称
     */
    const std::string& getNamespaceName() const { return m_namespaceName; }

    /**
     * @brief 设置命名空间名称
     * @param name 命名空间名称
     */
    void setNamespaceName(const std::string& name) { m_namespaceName = name; }

    /**
     * @brief 添加子命名空间
     * @param namespaceNode 子命名空间节点
     */
    void addSubNamespace(std::unique_ptr<NamespaceNode> namespaceNode);

    /**
     * @brief 获取子命名空间列表
     * @return 子命名空间列表
     */
    const std::vector<NamespaceNode*>& getSubNamespaces() const { return m_subNamespaces; }

    /**
     * @brief 查找子命名空间
     * @param name 命名空间名称
     * @return 子命名空间节点
     */
    NamespaceNode* findSubNamespace(const std::string& name) const;

    /**
     * @brief 添加模板定义
     * @param name 模板名称
     * @param templateNode 模板节点
     */
    void addTemplate(const std::string& name, std::unique_ptr<BaseNode> templateNode);

    /**
     * @brief 获取模板定义
     * @param name 模板名称
     * @return 模板节点
     */
    BaseNode* getTemplate(const std::string& name) const;

    /**
     * @brief 检查是否有模板
     * @param name 模板名称
     * @return 是否有该模板
     */
    bool hasTemplate(const std::string& name) const;

    /**
     * @brief 添加自定义定义
     * @param name 自定义名称
     * @param customNode 自定义节点
     */
    void addCustom(const std::string& name, std::unique_ptr<BaseNode> customNode);

    /**
     * @brief 获取自定义定义
     * @param name 自定义名称
     * @return 自定义节点
     */
    BaseNode* getCustom(const std::string& name) const;

    /**
     * @brief 检查是否有自定义
     * @param name 自定义名称
     * @return 是否有该自定义
     */
    bool hasCustom(const std::string& name) const;

    /**
     * @brief 添加变量定义
     * @param name 变量名称
     * @param value 变量值
     */
    void addVariable(const std::string& name, const std::string& value);

    /**
     * @brief 获取变量值
     * @param name 变量名称
     * @return 变量值
     */
    std::string getVariable(const std::string& name) const;

    /**
     * @brief 检查是否有变量
     * @param name 变量名称
     * @return 是否有该变量
     */
    bool hasVariable(const std::string& name) const;

    /**
     * @brief 获取所有模板
     * @return 模板映射
     */
    const std::unordered_map<std::string, std::unique_ptr<BaseNode>>& getTemplates() const { return m_templates; }

    /**
     * @brief 获取所有自定义
     * @return 自定义映射
     */
    const std::unordered_map<std::string, std::unique_ptr<BaseNode>>& getCustoms() const { return m_customs; }

    /**
     * @brief 获取所有变量
     * @return 变量映射
     */
    const std::unordered_map<std::string, std::string>& getVariables() const { return m_variables; }

    /**
     * @brief 检查是否为根命名空间
     * @return 是否为根命名空间
     */
    bool isRootNamespace() const { return m_namespaceName.empty(); }

    /**
     * @brief 检查是否为嵌套命名空间
     * @return 是否为嵌套命名空间
     */
    bool isNestedNamespace() const { return !m_subNamespaces.empty(); }

    /**
     * @brief 获取完整命名空间路径
     * @return 完整路径
     */
    std::string getFullPath() const;

    /**
     * @brief 解析命名空间路径
     * @param path 路径字符串
     * @return 命名空间节点
     */
    NamespaceNode* resolvePath(const std::string& path) const;

    /**
     * @brief 克隆节点
     * @return 克隆的节点
     */
    std::unique_ptr<BaseNode> clone() const override;

    /**
     * @brief 接受访问者
     * @param visitor 访问者
     */
    void accept(NodeVisitor* visitor) override;

    /**
     * @brief 获取节点字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;

    /**
     * @brief 检查命名空间名称是否有效
     * @param name 名称
     * @return 是否有效
     */
    static bool isValidNamespaceName(const std::string& name);

    /**
     * @brief 分割命名空间路径
     * @param path 路径
     * @return 路径组件列表
     */
    static std::vector<std::string> splitNamespacePath(const std::string& path);

private:
    std::string m_namespaceName;
    std::vector<NamespaceNode*> m_subNamespaces;
    std::unordered_map<std::string, std::unique_ptr<BaseNode>> m_templates;
    std::unordered_map<std::string, std::unique_ptr<BaseNode>> m_customs;
    std::unordered_map<std::string, std::string> m_variables;
    NamespaceNode* m_parent;
};

} // namespace CHTL