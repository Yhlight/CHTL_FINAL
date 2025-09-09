#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

enum class NodeType {
    // 基础节点
    ROOT,           // 根节点
    ELEMENT,        // 元素节点
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    
    // 模板节点
    TEMPLATE_STYLE, // 样式组模板
    TEMPLATE_ELEMENT, // 元素模板
    TEMPLATE_VAR,   // 变量组模板
    
    // 自定义节点
    CUSTOM_STYLE,   // 自定义样式组
    CUSTOM_ELEMENT, // 自定义元素
    CUSTOM_VAR,     // 自定义变量组
    
    // 其他节点
    STYLE,          // 样式节点
    SCRIPT,         // 脚本节点
    ORIGIN,         // 原始嵌入节点
    IMPORT,         // 导入节点
    CONFIG,         // 配置节点
    NAMESPACE,      // 命名空间节点
    OPERATOR        // 操作符节点
};

class BaseNode : public std::enable_shared_from_this<BaseNode> {
public:
    BaseNode(NodeType type, const std::string& name = "");
    virtual ~BaseNode() = default;
    
    // 基本属性
    NodeType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children_; }
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value);
    const std::string& getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& getAttributes() const { return attributes_; }
    
    // 父节点管理
    std::shared_ptr<BaseNode> getParent() const { return parent_.lock(); }
    void setParent(std::weak_ptr<BaseNode> parent) { parent_ = parent; }
    
    // 位置信息
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    void setPosition(size_t line, size_t column) { line_ = line; column_ = column; }
    
    // 虚函数
    virtual std::string toString() const;
    virtual std::string toHTML() const = 0;
    virtual std::string toCSS() const = 0;
    virtual std::string toJS() const = 0;
    
    // 查找子节点
    std::vector<std::shared_ptr<BaseNode>> findChildren(NodeType type) const;
    std::shared_ptr<BaseNode> findFirstChild(NodeType type) const;
    std::shared_ptr<BaseNode> findChildByName(const std::string& name) const;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const = 0;
    
protected:
    NodeType type_;
    std::string name_;
    std::vector<std::shared_ptr<BaseNode>> children_;
    std::unordered_map<std::string, std::string> attributes_;
    std::weak_ptr<BaseNode> parent_;
    size_t line_;
    size_t column_;
};

} // namespace CHTL