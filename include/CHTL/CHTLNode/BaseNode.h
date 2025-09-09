#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 节点类型枚举
 */
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
    OPERATOR        // 操作符节点
};

/**
 * @brief 基础节点类
 * 所有CHTL AST节点的基类
 */
class BaseNode : public std::enable_shared_from_this<BaseNode> {
public:
    BaseNode(NodeType type);
    virtual ~BaseNode();
    
    // 节点类型
    NodeType getType() const;
    
    // 节点属性
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    const std::unordered_map<std::string, std::string>& getAttributes() const;
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const;
    void clearChildren();
    
    // 父节点管理
    void setParent(std::shared_ptr<BaseNode> parent);
    std::shared_ptr<BaseNode> getParent() const;
    
    // 节点内容
    void setContent(const std::string& content);
    std::string getContent() const;
    
    // 节点名称
    void setName(const std::string& name);
    std::string getName() const;
    
    // 位置信息
    void setPosition(size_t line, size_t column);
    size_t getLine() const;
    size_t getColumn() const;
    
    // 访问者模式
    virtual void accept(class NodeVisitor& visitor) = 0;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const = 0;
    
    // 节点验证
    virtual bool validate() const;
    
    // 调试信息
    virtual std::string toString() const;

protected:
    NodeType m_type;
    std::string m_name;
    std::string m_content;
    std::unordered_map<std::string, std::string> m_attributes;
    std::vector<std::shared_ptr<BaseNode>> m_children;
    std::weak_ptr<BaseNode> m_parent;
    size_t m_line;
    size_t m_column;
};

/**
 * @brief 节点访问者接口
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
};

} // namespace CHTL