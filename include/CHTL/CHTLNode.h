#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>

namespace CHTL {

/**
 * @brief 节点类型枚举
 */
enum class NodeType {
    // 基础节点
    ROOT,           // 根节点
    ELEMENT,        // 元素节点
    TEXT,           // 文本节点
    COMMENT,        // 注释节点
    ATTRIBUTE,      // 属性节点
    
    // 模板节点
    TEMPLATE,       // 模板节点
    STYLE_TEMPLATE, // 样式模板
    ELEMENT_TEMPLATE, // 元素模板
    VAR_TEMPLATE,   // 变量模板
    
    // 自定义节点
    CUSTOM,         // 自定义节点
    CUSTOM_STYLE,   // 自定义样式
    CUSTOM_ELEMENT, // 自定义元素
    CUSTOM_VAR,     // 自定义变量
    
    // 原始嵌入节点
    ORIGIN,         // 原始嵌入节点
    ORIGIN_HTML,    // HTML原始嵌入
    ORIGIN_CSS,     // CSS原始嵌入
    ORIGIN_JS,      // JS原始嵌入
    
    // 导入节点
    IMPORT,         // 导入节点
    IMPORT_HTML,    // HTML导入
    IMPORT_CSS,     // CSS导入
    IMPORT_JS,      // JS导入
    IMPORT_CHTL,    // CHTL导入
    IMPORT_CJMOD,   // CJMOD导入
    
    // 配置节点
    CONFIGURATION,  // 配置节点
    NAMESPACE,      // 命名空间节点
    
    // 样式和脚本节点
    STYLE,          // 样式节点
    SCRIPT,         // 脚本节点
    
    // 操作节点
    DELETE_OP,      // 删除操作
    INSERT_OP,      // 插入操作
    INHERIT_OP,     // 继承操作
    
    // 表达式节点
    EXPRESSION,     // 表达式节点
    BINARY_OP,      // 二元操作
    UNARY_OP,       // 一元操作
    CONDITIONAL,    // 条件表达式
    REFERENCE,      // 引用节点
    VARIABLE,       // 变量节点
    FUNCTION_CALL,  // 函数调用
    
    // 特殊节点
    PLACEHOLDER,    // 占位符节点
    ERROR           // 错误节点
};

/**
 * @brief 属性值类型
 */
using AttributeValue = std::variant<std::string, int, double, bool>;

/**
 * @brief CHTL AST节点基类
 */
class CHTLNode : public std::enable_shared_from_this<CHTLNode> {
public:
    CHTLNode(NodeType type, const std::string& name = "");
    virtual ~CHTLNode() = default;

    // 基本属性
    NodeType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    // 位置信息
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    void setPosition(size_t line, size_t column) { line_ = line; column_ = column; }

    // 子节点管理
    void addChild(std::shared_ptr<CHTLNode> child);
    void removeChild(std::shared_ptr<CHTLNode> child);
    const std::vector<std::shared_ptr<CHTLNode>>& getChildren() const { return children_; }
    std::vector<std::shared_ptr<CHTLNode>>& getChildren() { return children_; }

    // 属性管理
    void setAttribute(const std::string& key, const AttributeValue& value);
    AttributeValue getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    const std::unordered_map<std::string, AttributeValue>& getAttributes() const { return attributes_; }

    // 文本内容
    const std::string& getTextContent() const { return text_content_; }
    void setTextContent(const std::string& content) { text_content_ = content; }

    // 父节点
    std::shared_ptr<CHTLNode> getParent() const { return parent_.lock(); }
    void setParent(std::shared_ptr<CHTLNode> parent) { parent_ = parent; }

    // 命名空间
    const std::string& getNamespace() const { return namespace_; }
    void setNamespace(const std::string& ns) { namespace_ = ns; }

    // 模板相关
    bool isTemplate() const;
    bool isCustom() const;
    bool isOrigin() const;
    bool isImport() const;

    // 访问者模式
    virtual void accept(class CHTLNodeVisitor& visitor) = 0;

    // 克隆节点
    virtual std::shared_ptr<CHTLNode> clone() const = 0;

    // 调试信息
    std::string toString() const;
    std::string toDebugString() const;

protected:
    NodeType type_;
    std::string name_;
    size_t line_;
    size_t column_;
    std::string text_content_;
    std::string namespace_;
    std::unordered_map<std::string, AttributeValue> attributes_;
    std::vector<std::shared_ptr<CHTLNode>> children_;
    std::weak_ptr<CHTLNode> parent_;
};

/**
 * @brief 元素节点
 */
class ElementNode : public CHTLNode {
public:
    ElementNode(const std::string& tag_name);
    ~ElementNode() override = default;

    const std::string& getTagName() const { return getName(); }
    void setTagName(const std::string& tag_name) { setName(tag_name); }

    void accept(class CHTLNodeVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
};

/**
 * @brief 文本节点
 */
class TextNode : public CHTLNode {
public:
    TextNode(const std::string& content);
    ~TextNode() override = default;

    void accept(class CHTLNodeVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
};

/**
 * @brief 注释节点
 */
class CommentNode : public CHTLNode {
public:
    CommentNode(const std::string& content);
    ~CommentNode() override = default;

    void accept(class CHTLNodeVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;
};

/**
 * @brief 模板节点
 */
class TemplateNode : public CHTLNode {
public:
    TemplateNode(const std::string& template_name, const std::string& template_type);
    ~TemplateNode() override = default;

    const std::string& getTemplateType() const { return template_type_; }
    void setTemplateType(const std::string& type) { template_type_ = type; }

    void accept(class CHTLNodeVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;

private:
    std::string template_type_;
};

/**
 * @brief 自定义节点
 */
class CustomNode : public CHTLNode {
public:
    CustomNode(const std::string& custom_name, const std::string& custom_type);
    ~CustomNode() override = default;

    const std::string& getCustomType() const { return custom_type_; }
    void setCustomType(const std::string& type) { custom_type_ = type; }

    void accept(class CHTLNodeVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;

private:
    std::string custom_type_;
};

/**
 * @brief 样式节点
 */
class StyleNode : public CHTLNode {
public:
    StyleNode();
    ~StyleNode() override = default;

    bool isLocal() const { return is_local_; }
    void setLocal(bool local) { is_local_ = local; }

    void accept(class CHTLNodeVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;

private:
    bool is_local_;
};

/**
 * @brief 脚本节点
 */
class ScriptNode : public CHTLNode {
public:
    ScriptNode();
    ~ScriptNode() override = default;

    bool isLocal() const { return is_local_; }
    void setLocal(bool local) { is_local_ = local; }

    void accept(class CHTLNodeVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;

private:
    bool is_local_;
};

/**
 * @brief 原始嵌入节点
 */
class OriginNode : public CHTLNode {
public:
    OriginNode(const std::string& origin_type);
    ~OriginNode() override = default;

    const std::string& getOriginType() const { return origin_type_; }
    void setOriginType(const std::string& type) { origin_type_ = type; }

    void accept(class CHTLNodeVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;

private:
    std::string origin_type_;
};

/**
 * @brief 导入节点
 */
class ImportNode : public CHTLNode {
public:
    ImportNode(const std::string& import_type, const std::string& path);
    ~ImportNode() override = default;

    const std::string& getImportType() const { return import_type_; }
    void setImportType(const std::string& type) { import_type_ = type; }
    
    const std::string& getPath() const { return path_; }
    void setPath(const std::string& path) { path_ = path; }

    void accept(class CHTLNodeVisitor& visitor) override;
    std::shared_ptr<CHTLNode> clone() const override;

private:
    std::string import_type_;
    std::string path_;
};

/**
 * @brief 节点访问者接口
 */
class CHTLNodeVisitor {
public:
    virtual ~CHTLNodeVisitor() = default;
    
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(CommentNode& node) = 0;
    virtual void visit(TemplateNode& node) = 0;
    virtual void visit(CustomNode& node) = 0;
    virtual void visit(StyleNode& node) = 0;
    virtual void visit(ScriptNode& node) = 0;
    virtual void visit(OriginNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
};

} // namespace CHTL