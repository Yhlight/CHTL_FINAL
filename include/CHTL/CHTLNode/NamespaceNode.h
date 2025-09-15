#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 命名空间节点类
 * 
 * 表示CHTL命名空间，用于防止模块污染
 */
class NamespaceNode : public BaseNode {
public:
    /**
     * @brief 构造函数
     * @param name 命名空间名称
     */
    explicit NamespaceNode(const std::string& name = "");
    
    /**
     * @brief 析构函数
     */
    ~NamespaceNode() override;
    
    /**
     * @brief 设置命名空间名称
     * @param name 命名空间名称
     */
    void setNamespaceName(const std::string& name);
    
    /**
     * @brief 获取命名空间名称
     * @return 命名空间名称
     */
    const std::string& getNamespaceName() const;
    
    /**
     * @brief 设置父命名空间
     * @param parent 父命名空间
     */
    void setParentNamespace(const std::string& parent);
    
    /**
     * @brief 获取父命名空间
     * @return 父命名空间
     */
    const std::string& getParentNamespace() const;
    
    /**
     * @brief 检查是否有父命名空间
     * @return 是否有父命名空间
     */
    bool hasParentNamespace() const;
    
    /**
     * @brief 添加子命名空间
     * @param child 子命名空间
     */
    void addChildNamespace(std::shared_ptr<NamespaceNode> child);
    
    /**
     * @brief 获取子命名空间列表
     * @return 子命名空间列表
     */
    const std::vector<std::shared_ptr<NamespaceNode>>& getChildNamespaces() const;
    
    /**
     * @brief 添加符号
     * @param name 符号名称
     * @param type 符号类型
     * @param value 符号值
     */
    void addSymbol(const std::string& name, const std::string& type, const std::string& value = "");
    
    /**
     * @brief 获取符号
     * @param name 符号名称
     * @return 符号值
     */
    std::string getSymbol(const std::string& name) const;
    
    /**
     * @brief 检查是否有符号
     * @param name 符号名称
     * @return 是否有符号
     */
    bool hasSymbol(const std::string& name) const;
    
    /**
     * @brief 获取所有符号
     * @return 符号映射
     */
    const std::unordered_map<std::string, std::pair<std::string, std::string>>& getSymbols() const;
    
    /**
     * @brief 设置命名空间内容
     * @param content 命名空间内容
     */
    void setNamespaceContent(const std::string& content);
    
    /**
     * @brief 获取命名空间内容
     * @return 命名空间内容
     */
    const std::string& getNamespaceContent() const;
    
    /**
     * @brief 设置是否合并
     * @param merge 是否合并
     */
    void setMerge(bool merge);
    
    /**
     * @brief 是否合并
     * @return 是否合并
     */
    bool isMerge() const;
    
    /**
     * @brief 克隆节点
     * @return 克隆的节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 转换为字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;
    
    /**
     * @brief 接受访问者
     * @param visitor 访问者
     */
    void accept(NodeVisitor& visitor) override {}

private:
    std::string m_namespaceName;
    std::string m_parentNamespace;
    std::vector<std::shared_ptr<NamespaceNode>> m_childNamespaces;
    std::unordered_map<std::string, std::pair<std::string, std::string>> m_symbols;
    std::string m_namespaceContent;
    bool m_merge;
};

} // namespace CHTL