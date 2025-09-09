#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>

namespace CHTL {

// 前向声明
class CHTLNode;

/**
 * @brief 节点类型枚举
 */
enum class NodeType {
    // 基础节点
    ROOT,           // 根节点
    ELEMENT,        // HTML元素节点
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    
    // 样式相关
    STYLE,          // 样式节点
    STYLE_PROPERTY, // 样式属性节点
    STYLE_SELECTOR, // 样式选择器节点
    
    // 脚本相关
    SCRIPT,         // 脚本节点
    
    // 模板相关
    TEMPLATE,       // 模板节点
    TEMPLATE_STYLE, // 样式组模板
    TEMPLATE_ELEMENT, // 元素模板
    TEMPLATE_VAR,   // 变量组模板
    
    // 自定义相关
    CUSTOM,         // 自定义节点
    CUSTOM_STYLE,   // 自定义样式组
    CUSTOM_ELEMENT, // 自定义元素
    CUSTOM_VAR,     // 自定义变量组
    
    // 导入相关
    IMPORT,         // 导入节点
    IMPORT_HTML,    // HTML导入
    IMPORT_CSS,     // CSS导入
    IMPORT_JS,      // JS导入
    IMPORT_CHTL,    // CHTL导入
    
    // 原始嵌入
    ORIGIN,         // 原始嵌入节点
    ORIGIN_HTML,    // HTML原始嵌入
    ORIGIN_CSS,     // CSS原始嵌入
    ORIGIN_JS,      // JS原始嵌入
    
    // 配置相关
    CONFIGURATION,  // 配置节点
    NAMESPACE,      // 命名空间节点
    
    // 操作相关
    OPERATOR,       // 操作符节点
    DELETE_OP,      // 删除操作
    INSERT_OP,      // 插入操作
    USE_OP,         // 使用操作
    EXCEPT_OP,      // 约束操作
};

/**
 * @brief 属性值类型
 */
using AttributeValue = std::variant<std::string, int, double, bool>;

/**
 * @brief 属性映射表
 */
using AttributeMap = std::unordered_map<std::string, AttributeValue>;

/**
 * @brief CHTL AST节点基类
 */
class CHTLNode {
public:
    CHTLNode(NodeType type, const std::string& name = "");
    virtual ~CHTLNode() = default;

    // 基本属性
    NodeType getType() const { return m_type; }
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    // 子节点管理
    void addChild(std::shared_ptr<CHTLNode> child);
    void removeChild(std::shared_ptr<CHTLNode> child);
    const std::vector<std::shared_ptr<CHTLNode>>& getChildren() const;
    std::vector<std::shared_ptr<CHTLNode>>& getChildren();
    
    // 属性管理
    void setAttribute(const std::string& key, const AttributeValue& value);
    AttributeValue getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    const AttributeMap& getAttributes() const;
    
    // 文本内容
    void setTextContent(const std::string& text);
    const std::string& getTextContent() const;
    
    // 位置信息
    void setPosition(size_t line, size_t column);
    size_t getLine() const { return m_line; }
    size_t getColumn() const { return m_column; }
    
    // 命名空间
    void setNamespace(const std::string& ns);
    const std::string& getNamespace() const;
    
    // 虚函数 - 子类可重写
    virtual std::string toString() const;
    virtual std::string toHTML() const;
    virtual std::string toCSS() const;
    virtual std::string toJS() const;
    
    // 查找功能
    std::shared_ptr<CHTLNode> findChild(const std::string& name) const;
    std::vector<std::shared_ptr<CHTLNode>> findChildren(const std::string& name) const;
    std::shared_ptr<CHTLNode> findChildByType(NodeType type) const;
    std::vector<std::shared_ptr<CHTLNode>> findChildrenByType(NodeType type) const;

protected:
    NodeType m_type;
    std::string m_name;
    std::vector<std::shared_ptr<CHTLNode>> m_children;
    AttributeMap m_attributes;
    std::string m_textContent;
    size_t m_line;
    size_t m_column;
    std::string m_namespace;
};

/**
 * @brief 元素节点
 */
class ElementNode : public CHTLNode {
public:
    ElementNode(const std::string& tagName);
    virtual ~ElementNode() = default;
    
    const std::string& getTagName() const;
    void setTagName(const std::string& tagName);
    
    bool isSelfClosing() const;
    void setSelfClosing(bool selfClosing);
    
    virtual std::string toHTML() const override;
};

/**
 * @brief 文本节点
 */
class TextNode : public CHTLNode {
public:
    TextNode(const std::string& text);
    virtual ~TextNode() = default;
    
    virtual std::string toHTML() const override;
};

/**
 * @brief 样式节点
 */
class StyleNode : public CHTLNode {
public:
    StyleNode();
    virtual ~StyleNode() = default;
    
    void addStyleProperty(const std::string& property, const std::string& value);
    void addStyleRule(const std::string& selector, const std::unordered_map<std::string, std::string>& properties);
    
    virtual std::string toCSS() const override;
    
private:
    std::unordered_map<std::string, std::string> m_properties;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_rules;
};

/**
 * @brief 脚本节点
 */
class ScriptNode : public CHTLNode {
public:
    ScriptNode();
    virtual ~ScriptNode() = default;
    
    void addScript(const std::string& script);
    
    virtual std::string toJS() const override;
    
private:
    std::vector<std::string> m_scripts;
};

/**
 * @brief 模板节点
 */
class TemplateNode : public CHTLNode {
public:
    TemplateNode(NodeType type, const std::string& name);
    virtual ~TemplateNode() = default;
    
    void setTemplateType(const std::string& type);
    const std::string& getTemplateType() const;
    
    virtual std::string toString() const override;
    
private:
    std::string m_templateType;
};

/**
 * @brief 导入节点
 */
class ImportNode : public CHTLNode {
public:
    ImportNode(NodeType type, const std::string& path);
    virtual ~ImportNode() = default;
    
    void setImportPath(const std::string& path);
    const std::string& getImportPath() const;
    
    void setAlias(const std::string& alias);
    const std::string& getAlias() const;
    
    virtual std::string toString() const override;
    
private:
    std::string m_importPath;
    std::string m_alias;
};

} // namespace CHTL