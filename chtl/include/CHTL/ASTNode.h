#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>

namespace CHTL {

/**
 * AST节点类型枚举
 */
enum class ASTNodeType {
    // 基础节点
    ROOT,           // 根节点
    ELEMENT,        // 元素节点
    TEXT,           // 文本节点
    ATTRIBUTE,      // 属性节点
    COMMENT,        // 注释节点
    
    // 样式相关
    STYLE_BLOCK,    // 样式块
    STYLE_PROPERTY, // 样式属性
    STYLE_SELECTOR, // 样式选择器
    STYLE_VALUE,    // 样式值
    
    // 脚本相关
    SCRIPT_BLOCK,   // 脚本块
    SCRIPT_STATEMENT, // 脚本语句
    
    // 模板相关
    TEMPLATE,       // 模板
    TEMPLATE_STYLE, // 样式模板
    TEMPLATE_ELEMENT, // 元素模板
    TEMPLATE_VAR,   // 变量模板
    
    // 自定义相关
    CUSTOM,         // 自定义
    CUSTOM_STYLE,   // 自定义样式
    CUSTOM_ELEMENT, // 自定义元素
    CUSTOM_VAR,     // 自定义变量
    
    // 原始嵌入
    ORIGIN,         // 原始嵌入
    ORIGIN_HTML,    // HTML原始嵌入
    ORIGIN_CSS,     // CSS原始嵌入
    ORIGIN_JS,      // JS原始嵌入
    
    // 导入和命名空间
    IMPORT,         // 导入
    NAMESPACE,      // 命名空间
    
    // 配置
    CONFIGURATION,  // 配置
    CONFIG_ITEM,    // 配置项
    
    // 约束
    CONSTRAINT,     // 约束
    EXCEPT,         // except约束
    
    // 使用
    USE,            // use语句
};

/**
 * 属性值类型
 */
using AttributeValue = std::variant<std::string, double, bool>;

/**
 * AST节点基类
 */
class ASTNode {
public:
    ASTNode(ASTNodeType type, const std::string& name = "");
    virtual ~ASTNode() = default;
    
    // 基本信息
    ASTNodeType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    // 位置信息
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    void setPosition(size_t line, size_t column) { line_ = line; column_ = column; }
    
    // 子节点管理
    void addChild(std::shared_ptr<ASTNode> child);
    void removeChild(std::shared_ptr<ASTNode> child);
    const std::vector<std::shared_ptr<ASTNode>>& getChildren() const { return children_; }
    std::vector<std::shared_ptr<ASTNode>>& getChildren() { return children_; }
    
    // 属性管理
    void setAttribute(const std::string& key, const AttributeValue& value);
    AttributeValue getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    const std::unordered_map<std::string, AttributeValue>& getAttributes() const { return attributes_; }
    
    // 文本内容
    const std::string& getText() const { return text_; }
    void setText(const std::string& text) { text_ = text; }
    
    // 访问者模式
    virtual void accept(class ASTVisitor& visitor) = 0;
    
    // 调试信息
    std::string toString() const;
    std::string toTreeString(int indent = 0) const;
    
protected:
    ASTNodeType type_;
    std::string name_;
    size_t line_;
    size_t column_;
    std::vector<std::shared_ptr<ASTNode>> children_;
    std::unordered_map<std::string, AttributeValue> attributes_;
    std::string text_;
};

/**
 * 元素节点
 */
class ElementNode : public ASTNode {
public:
    ElementNode(const std::string& tagName);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getTagName() const { return getName(); }
    void setTagName(const std::string& tagName) { setName(tagName); }
    
    bool isSelfClosing() const;
    void setSelfClosing(bool selfClosing) { selfClosing_ = selfClosing; }
    
private:
    bool selfClosing_;
};

/**
 * 文本节点
 */
class TextNode : public ASTNode {
public:
    TextNode(const std::string& content);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getContent() const { return getText(); }
    void setContent(const std::string& content) { setText(content); }
};

/**
 * 样式节点
 */
class StyleNode : public ASTNode {
public:
    StyleNode();
    void accept(ASTVisitor& visitor) override;
    
    void addProperty(const std::string& property, const std::string& value);
    const std::unordered_map<std::string, std::string>& getProperties() const { return properties_; }
    
private:
    std::unordered_map<std::string, std::string> properties_;
};

/**
 * 脚本节点
 */
class ScriptNode : public ASTNode {
public:
    ScriptNode();
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getScript() const { return getText(); }
    void setScript(const std::string& script) { setText(script); }
};

/**
 * 模板节点
 */
class TemplateNode : public ASTNode {
public:
    TemplateNode(const std::string& templateType, const std::string& templateName);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getTemplateType() const { return templateType_; }
    const std::string& getTemplateName() const { return templateName_; }
    
private:
    std::string templateType_;
    std::string templateName_;
};

/**
 * 自定义节点
 */
class CustomNode : public ASTNode {
public:
    CustomNode(const std::string& customType, const std::string& customName);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getCustomType() const { return customType_; }
    const std::string& getCustomName() const { return customName_; }
    
private:
    std::string customType_;
    std::string customName_;
};

/**
 * 原始嵌入节点
 */
class OriginNode : public ASTNode {
public:
    OriginNode(const std::string& originType, const std::string& originName = "");
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getOriginType() const { return originType_; }
    const std::string& getOriginName() const { return originName_; }
    
private:
    std::string originType_;
    std::string originName_;
};

/**
 * 导入节点
 */
class ImportNode : public ASTNode {
public:
    ImportNode(const std::string& importType, const std::string& importPath);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getImportType() const { return importType_; }
    const std::string& getImportPath() const { return importPath_; }
    const std::string& getImportAlias() const { return importAlias_; }
    void setImportAlias(const std::string& alias) { importAlias_ = alias; }
    
private:
    std::string importType_;
    std::string importPath_;
    std::string importAlias_;
};

/**
 * 命名空间节点
 */
class NamespaceNode : public ASTNode {
public:
    NamespaceNode(const std::string& namespaceName);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getNamespaceName() const { return getName(); }
};

/**
 * 配置节点
 */
class ConfigurationNode : public ASTNode {
public:
    ConfigurationNode(const std::string& configName = "");
    void accept(ASTVisitor& visitor) override;
    
    void addConfigItem(const std::string& key, const std::string& value);
    const std::unordered_map<std::string, std::string>& getConfigItems() const { return configItems_; }
    
private:
    std::unordered_map<std::string, std::string> configItems_;
};

/**
 * 约束节点
 */
class ConstraintNode : public ASTNode {
public:
    ConstraintNode();
    void accept(ASTVisitor& visitor) override;
    
    void addException(const std::string& exception);
    const std::vector<std::string>& getExceptions() const { return exceptions_; }
    
private:
    std::vector<std::string> exceptions_;
};

/**
 * 使用节点
 */
class UseNode : public ASTNode {
public:
    UseNode(const std::string& useTarget);
    void accept(ASTVisitor& visitor) override;
    
    const std::string& getUseTarget() const { return getText(); }
};

} // namespace CHTL