#pragma once

#include "CHTLLexer.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 节点类型枚举
 */
enum class NodeType {
    // 基础节点
    ELEMENT,        // HTML 元素
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    
    // 模板节点
    TEMPLATE_STYLE,     // 样式组模板
    TEMPLATE_ELEMENT,   // 元素模板
    TEMPLATE_VAR,       // 变量组模板
    
    // 自定义节点
    CUSTOM_STYLE,       // 自定义样式组
    CUSTOM_ELEMENT,     // 自定义元素
    CUSTOM_VAR,         // 自定义变量组
    
    // 特殊节点
    STYLE,          // 局部样式块
    SCRIPT,         // 局部脚本块
    ORIGIN,         // 原始嵌入
    IMPORT,         // 导入
    CONFIGURATION,  // 配置组
    NAMESPACE,      // 命名空间
    OPERATOR        // 操作符（delete, insert, use 等）
};

/**
 * @brief 基础节点类
 */
class BaseNode {
public:
    BaseNode(NodeType type, int line = 0, int column = 0);
    virtual ~BaseNode() = default;

    // 基本属性
    NodeType getType() const { return m_type; }
    int getLine() const { return m_line; }
    int getColumn() const { return m_column; }
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const;
    void clearChildren();
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& getAttributes() const;
    
    // 访问者模式
    virtual void accept(class NodeVisitor& visitor) = 0;

protected:
    NodeType m_type;
    int m_line;
    int m_column;
    std::vector<std::shared_ptr<BaseNode>> m_children;
    std::unordered_map<std::string, std::string> m_attributes;
};

/**
 * @brief 元素节点
 */
class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName, int line = 0, int column = 0);
    
    std::string getTagName() const { return m_tagName; }
    void setTagName(const std::string& tagName) { m_tagName = tagName; }
    
    void accept(class NodeVisitor& visitor) override;

private:
    std::string m_tagName;
};

/**
 * @brief 文本节点
 */
class TextNode : public BaseNode {
public:
    TextNode(const std::string& content, int line = 0, int column = 0);
    
    std::string getContent() const { return m_content; }
    void setContent(const std::string& content) { m_content = content; }
    
    void accept(class NodeVisitor& visitor) override;

private:
    std::string m_content;
};

/**
 * @brief 注释节点
 */
class CommentNode : public BaseNode {
public:
    CommentNode(const std::string& content, bool isGenerator = false, int line = 0, int column = 0);
    
    std::string getContent() const { return m_content; }
    bool isGenerator() const { return m_isGenerator; }
    
    void accept(class NodeVisitor& visitor) override;

private:
    std::string m_content;
    bool m_isGenerator;
};

/**
 * @brief 模板节点基类
 */
class TemplateNode : public BaseNode {
public:
    TemplateNode(NodeType type, const std::string& name, int line = 0, int column = 0);
    
    std::string getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    void accept(class NodeVisitor& visitor) override;

private:
    std::string m_name;
};

/**
 * @brief 自定义节点基类
 */
class CustomNode : public BaseNode {
public:
    CustomNode(NodeType type, const std::string& name, int line = 0, int column = 0);
    
    std::string getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    void accept(class NodeVisitor& visitor) override;

private:
    std::string m_name;
};

/**
 * @brief 样式节点
 */
class StyleNode : public BaseNode {
public:
    StyleNode(int line = 0, int column = 0);
    
    void accept(class NodeVisitor& visitor) override;
};

/**
 * @brief 脚本节点
 */
class ScriptNode : public BaseNode {
public:
    ScriptNode(int line = 0, int column = 0);
    
    void accept(class NodeVisitor& visitor) override;
};

/**
 * @brief 原始嵌入节点
 */
class OriginNode : public BaseNode {
public:
    OriginNode(const std::string& type, const std::string& name = "", int line = 0, int column = 0);
    
    std::string getOriginType() const { return m_originType; }
    std::string getName() const { return m_name; }
    
    void accept(class NodeVisitor& visitor) override;

private:
    std::string m_originType;
    std::string m_name;
};

/**
 * @brief 导入节点
 */
class ImportNode : public BaseNode {
public:
    ImportNode(const std::string& importType, const std::string& path, 
               const std::string& alias = "", int line = 0, int column = 0);
    
    std::string getImportType() const { return m_importType; }
    std::string getPath() const { return m_path; }
    std::string getAlias() const { return m_alias; }
    
    void accept(class NodeVisitor& visitor) override;

private:
    std::string m_importType;
    std::string m_path;
    std::string m_alias;
};

/**
 * @brief 配置节点
 */
class ConfigNode : public BaseNode {
public:
    ConfigNode(const std::string& name = "", int line = 0, int column = 0);
    
    std::string getName() const { return m_name; }
    bool isNamed() const { return !m_name.empty(); }
    
    void accept(class NodeVisitor& visitor) override;

private:
    std::string m_name;
};

/**
 * @brief 命名空间节点
 */
class NamespaceNode : public BaseNode {
public:
    NamespaceNode(const std::string& name, int line = 0, int column = 0);
    
    std::string getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    void accept(class NodeVisitor& visitor) override;

private:
    std::string m_name;
};

/**
 * @brief 操作符节点
 */
class OperatorNode : public BaseNode {
public:
    enum class OperatorType {
        DELETE,
        INSERT,
        USE,
        INHERIT
    };
    
    OperatorNode(OperatorType opType, int line = 0, int column = 0);
    
    OperatorType getOperatorType() const { return m_operatorType; }
    
    void accept(class NodeVisitor& visitor) override;

private:
    OperatorType m_operatorType;
};

/**
 * @brief 节点访问者接口
 */
class NodeVisitor {
public:
    virtual ~NodeVisitor() = default;
    
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(TemplateNode& node) = 0;
    virtual void visit(CustomNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(ScriptNode& node) = 0;
    virtual void visit(OriginNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
    virtual void visit(ConfigNode& node) = 0;
    virtual void visit(NamespaceNode& node) = 0;
    virtual void visit(OperatorNode& node) = 0;
};

} // namespace CHTL