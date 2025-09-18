#ifndef BASE_NODE_H
#define BASE_NODE_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * AST节点基类
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
        OPERATOR,       // 操作符节点
        EXPRESSION      // 表达式节点
    };

    BaseNode(NodeType type);
    virtual ~BaseNode() = default;

    // 获取节点类型
    NodeType getType() const { return m_type; }
    
    // 获取/设置节点值
    const std::string& getValue() const { return m_value; }
    void setValue(const std::string& value) { m_value = value; }
    
    // 获取/设置属性
    const std::string& getAttribute(const std::string& name) const;
    void setAttribute(const std::string& name, const std::string& value);
    bool hasAttribute(const std::string& name) const;
    void removeAttribute(const std::string& name);
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return m_children; }
    std::vector<std::shared_ptr<BaseNode>>& getChildren() { return m_children; }
    
    // 查找子节点
    std::vector<std::shared_ptr<BaseNode>> findChildren(NodeType type) const;
    std::shared_ptr<BaseNode> findFirstChild(NodeType type) const;
    
    // 获取/设置位置信息
    size_t getLine() const { return m_line; }
    size_t getColumn() const { return m_column; }
    void setPosition(size_t line, size_t column) { m_line = line; m_column = column; }
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const = 0;
    
    // 转换为字符串
    virtual std::string toString() const = 0;
    
    // 接受访问者
    virtual void accept(class NodeVisitor& visitor) = 0;

protected:
    NodeType m_type;
    std::string m_value;
    std::map<std::string, std::string> m_attributes;
    std::vector<std::shared_ptr<BaseNode>> m_children;
    size_t m_line;
    size_t m_column;
};

/**
 * 节点访问者接口
 */
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    virtual void visitElement(class ElementNode& node) = 0;
    virtual void visitText(class TextNode& node) = 0;
    virtual void visitComment(class CommentNode& node) = 0;
    virtual void visitTemplate(class TemplateNode& node) = 0;
    virtual void visitCustom(class CustomNode& node) = 0;
    virtual void visitStyle(class StyleNode& node) = 0;
    virtual void visitScript(class ScriptNode& node) = 0;
    virtual void visitOrigin(class OriginNode& node) = 0;
    virtual void visitImport(class ImportNode& node) = 0;
    virtual void visitConfig(class ConfigNode& node) = 0;
    virtual void visitNamespace(class NamespaceNode& node) = 0;
    virtual void visitOperator(class OperatorNode& node) = 0;
    virtual void visitExpression(class ExpressionNode& node) = 0;
};

} // namespace CHTL

#endif // BASE_NODE_H