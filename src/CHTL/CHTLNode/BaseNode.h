#ifndef CHTL_BASE_NODE_H
#define CHTL_BASE_NODE_H

#include "../Token.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {

enum class NodeType {
    ELEMENT,        // HTML元素节点
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    TEMPLATE,       // 模板节点
    CUSTOM,         // 自定义节点
    STYLE,          // 样式节点
    SCRIPT,         // 脚本节点
    ORIGIN,         // 原始嵌入节点
    IMPORT,         // 导入节点
    CONFIG,         // 配置节点
    NAMESPACE,      // 命名空间节点
    OPERATOR        // 操作节点
};

class BaseNode : public std::enable_shared_from_this<BaseNode> {
public:
    BaseNode(NodeType type, const std::string& name = "");
    virtual ~BaseNode() = default;
    
    // 基本属性
    NodeType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // 位置信息
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    void setPosition(size_t line, size_t column) { line_ = line; column_ = column; }
    
    // 父子关系
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children_; }
    std::shared_ptr<BaseNode> getParent() const { return parent_; }
    void setParent(std::shared_ptr<BaseNode> parent) { parent_ = parent; }
    
    // 属性管理
    void setAttribute(const std::string& name, const std::string& value);
    std::string getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    void removeAttribute(const std::string& name);
    const std::unordered_map<std::string, std::string>& getAttributes() const { return attributes_; }
    
    // 内容管理
    void setContent(const std::string& content) { content_ = content; }
    const std::string& getContent() const { return content_; }
    
    // 虚拟方法
    virtual std::string toString() const;
    virtual std::string toHTML() const = 0;
    virtual std::string toCSS() const { return ""; }
    virtual std::string toJS() const { return ""; }
    
    // 查找方法
    std::vector<std::shared_ptr<BaseNode>> findChildren(const std::string& name) const;
    std::shared_ptr<BaseNode> findFirstChild(const std::string& name) const;
    std::vector<std::shared_ptr<BaseNode>> findDescendants(const std::string& name) const;
    
    // 验证方法
    virtual bool isValid() const { return true; }
    
protected:
    NodeType type_;
    std::string name_;
    size_t line_;
    size_t column_;
    std::string content_;
    std::unordered_map<std::string, std::string> attributes_;
    std::vector<std::shared_ptr<BaseNode>> children_;
    std::weak_ptr<BaseNode> parent_;
    
    // 辅助方法
    std::string escapeHTML(const std::string& text) const;
    std::string escapeCSS(const std::string& text) const;
    std::string escapeJS(const std::string& text) const;
};

using NodePtr = std::shared_ptr<BaseNode>;
using NodeList = std::vector<NodePtr>;

} // namespace CHTL

#endif // CHTL_BASE_NODE_H