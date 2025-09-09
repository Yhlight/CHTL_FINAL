#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * @brief AST 节点基类
 * 
 * 所有 CHTL AST 节点的基类
 */
class BaseNode {
public:
    enum class NodeType {
        ELEMENT,        // 元素节点
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
        CONSTRAINT,     // 约束节点
        OPERATOR        // 操作符节点
    };
    
    BaseNode(NodeType type, size_t line = 0, size_t column = 0);
    virtual ~BaseNode() = default;
    
    // 获取节点类型
    NodeType getType() const { return type_; }
    
    // 获取位置信息
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    // 设置位置信息
    void setPosition(size_t line, size_t column) { line_ = line; column_ = column; }
    
    // 添加子节点
    void addChild(std::shared_ptr<BaseNode> child);
    
    // 获取子节点
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children_; }
    
    // 获取子节点数量
    size_t getChildCount() const { return children_.size(); }
    
    // 获取指定索引的子节点
    std::shared_ptr<BaseNode> getChild(size_t index) const;
    
    // 设置属性
    void setAttribute(const std::string& name, const std::string& value);
    
    // 获取属性
    std::string getAttribute(const std::string& name) const;
    
    // 检查是否有属性
    bool hasAttribute(const std::string& name) const;
    
    // 获取所有属性
    const std::map<std::string, std::string>& getAttributes() const { return attributes_; }
    
    // 设置值
    void setValue(const std::string& value) { value_ = value; }
    
    // 获取值
    const std::string& getValue() const { return value_; }
    
    // 转换为字符串（用于调试）
    virtual std::string toString() const;
    
    // 转换为 HTML（纯虚函数，由子类实现）
    virtual std::string toHTML() const = 0;
    
protected:
    NodeType type_;
    size_t line_;
    size_t column_;
    std::vector<std::shared_ptr<BaseNode>> children_;
    std::map<std::string, std::string> attributes_;
    std::string value_;
};

} // namespace CHTL