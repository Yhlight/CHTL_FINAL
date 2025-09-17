#ifndef CHTL_AST_NODE_H
#define CHTL_AST_NODE_H

#include "../CHTLLexer/CHTLToken.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

enum class CHTLASTNodeType {
    // 基础节点
    PROGRAM,
    STATEMENT,
    EXPRESSION,
    
    // HTML元素节点
    HTML_ELEMENT,
    TEXT_NODE,
    COMMENT_NODE,
    
    // 属性节点
    ATTRIBUTE,
    ATTRIBUTE_LIST,
    
    // 样式节点
    STYLE_BLOCK,
    STYLE_RULE,
    STYLE_PROPERTY,
    STYLE_SELECTOR,
    
    // 脚本节点
    SCRIPT_BLOCK,
    
    // 模板节点
    TEMPLATE_DECLARATION,
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    
    // 自定义节点
    CUSTOM_DECLARATION,
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    
    // 原始嵌入节点
    ORIGIN_NODE,
    ORIGIN_HTML,
    ORIGIN_STYLE,
    ORIGIN_JAVASCRIPT,
    
    // 导入节点
    IMPORT_NODE,
    IMPORT_HTML,
    IMPORT_STYLE,
    IMPORT_JAVASCRIPT,
    IMPORT_CHTL,
    IMPORT_CJMOD,
    
    // 命名空间节点
    NAMESPACE_NODE,
    
    // 配置节点
    CONFIGURATION_NODE,
    CONFIG_NAME,
    CONFIG_VALUE,
    
    // 信息节点
    INFO_NODE,
    EXPORT_NODE,
    
    // 操作节点
    DELETE_NODE,
    INSERT_NODE,
    INHERIT_NODE,
    
    // 约束节点
    EXCEPT_NODE,
    
    // 其他
    UNKNOWN
};

class CHTLASTNode {
public:
    CHTLASTNodeType type;
    CHTLToken token;
    std::vector<std::shared_ptr<CHTLASTNode>> children;
    std::unordered_map<std::string, std::shared_ptr<CHTLASTNode>> properties;
    
    CHTLASTNode(CHTLASTNodeType t = CHTLASTNodeType::UNKNOWN, const CHTLToken& tok = CHTLToken())
        : type(t), token(tok) {}
    
    virtual ~CHTLASTNode() = default;
    
    virtual std::string toString() const;
    virtual std::string toHTML() const = 0;
    
    void addChild(std::shared_ptr<CHTLASTNode> child);
    void setProperty(const std::string& key, std::shared_ptr<CHTLASTNode> value);
    std::shared_ptr<CHTLASTNode> getProperty(const std::string& key) const;
    
    bool hasChildren() const;
    size_t getChildCount() const;
    std::shared_ptr<CHTLASTNode> getChild(size_t index) const;
    
    bool hasProperty(const std::string& key) const;
    std::vector<std::string> getPropertyKeys() const;
};

// 具体节点类型
class ProgramNode : public CHTLASTNode {
public:
    ProgramNode() : CHTLASTNode(CHTLASTNodeType::PROGRAM) {}
    std::string toHTML() const override;
};

class HTMLElementNode : public CHTLASTNode {
public:
    std::string tagName;
    std::vector<std::shared_ptr<CHTLASTNode>> attributes;
    std::vector<std::shared_ptr<CHTLASTNode>> children;
    bool isSelfClosing;
    
    HTMLElementNode(const std::string& tag) 
        : CHTLASTNode(CHTLASTNodeType::HTML_ELEMENT), tagName(tag), isSelfClosing(false) {}
    
    std::string toHTML() const override;
};

class TextNode : public CHTLASTNode {
public:
    std::string content;
    bool isRaw;
    
    TextNode(const std::string& text, bool raw = false) 
        : CHTLASTNode(CHTLASTNodeType::TEXT_NODE), content(text), isRaw(raw) {}
    
    std::string toHTML() const override;
};

class AttributeNode : public CHTLASTNode {
public:
    std::string name;
    std::string value;
    bool hasValue;
    
    AttributeNode(const std::string& attrName) 
        : CHTLASTNode(CHTLASTNodeType::ATTRIBUTE), name(attrName), hasValue(false) {}
    
    std::string toHTML() const override;
};

class StyleBlockNode : public CHTLASTNode {
public:
    std::vector<std::shared_ptr<CHTLASTNode>> rules;
    bool isGlobal;
    
    StyleBlockNode(bool global = false) 
        : CHTLASTNode(CHTLASTNodeType::STYLE_BLOCK), isGlobal(global) {}
    
    std::string toHTML() const override;
};

class StyleRuleNode : public CHTLASTNode {
public:
    std::shared_ptr<CHTLASTNode> selector;
    std::vector<std::shared_ptr<CHTLASTNode>> properties;
    
    StyleRuleNode() : CHTLASTNode(CHTLASTNodeType::STYLE_RULE) {}
    
    std::string toHTML() const override;
};

class StylePropertyNode : public CHTLASTNode {
public:
    std::string name;
    std::string value;
    bool isImportant;
    
    StylePropertyNode(const std::string& propName, const std::string& propValue) 
        : CHTLASTNode(CHTLASTNodeType::STYLE_PROPERTY), name(propName), value(propValue), isImportant(false) {}
    
    std::string toHTML() const override;
};

class ScriptBlockNode : public CHTLASTNode {
public:
    std::string content;
    std::string type;
    bool isInline;
    
    ScriptBlockNode(const std::string& scriptContent) 
        : CHTLASTNode(CHTLASTNodeType::SCRIPT_BLOCK), content(scriptContent), type("text/javascript"), isInline(true) {}
    
    std::string toHTML() const override;
};

class TemplateDeclarationNode : public CHTLASTNode {
public:
    std::string templateType; // @Style, @Element, @Var
    std::string name;
    std::vector<std::shared_ptr<CHTLASTNode>> content;
    
    TemplateDeclarationNode(const std::string& type, const std::string& templateName) 
        : CHTLASTNode(CHTLASTNodeType::TEMPLATE_DECLARATION), templateType(type), name(templateName) {}
    
    std::string toHTML() const override;
};

class CustomDeclarationNode : public CHTLASTNode {
public:
    std::string customType; // @Style, @Element, @Var
    std::string name;
    std::vector<std::shared_ptr<CHTLASTNode>> content;
    
    CustomDeclarationNode(const std::string& type, const std::string& customName) 
        : CHTLASTNode(CHTLASTNodeType::CUSTOM_DECLARATION), customType(type), name(customName) {}
    
    std::string toHTML() const override;
};

class OriginNode : public CHTLASTNode {
public:
    std::string originType; // @Html, @Style, @JavaScript
    std::string name;
    std::string content;
    
    OriginNode(const std::string& type, const std::string& originName) 
        : CHTLASTNode(CHTLASTNodeType::ORIGIN_NODE), originType(type), name(originName) {}
    
    std::string toHTML() const override;
};

class ImportNode : public CHTLASTNode {
public:
    std::string importType; // @Html, @Style, @JavaScript, @Chtl, @CJmod
    std::string path;
    std::string alias;
    std::string from;
    
    ImportNode(const std::string& type, const std::string& importPath) 
        : CHTLASTNode(CHTLASTNodeType::IMPORT_NODE), importType(type), path(importPath) {}
    
    std::string toHTML() const override;
};

class NamespaceNode : public CHTLASTNode {
public:
    std::string name;
    std::vector<std::shared_ptr<CHTLASTNode>> content;
    
    NamespaceNode(const std::string& namespaceName) 
        : CHTLASTNode(CHTLASTNodeType::NAMESPACE_NODE), name(namespaceName) {}
    
    std::string toHTML() const override;
};

class ConfigurationNode : public CHTLASTNode {
public:
    std::string name;
    std::unordered_map<std::string, std::string> settings;
    
    ConfigurationNode(const std::string& configName = "") 
        : CHTLASTNode(CHTLASTNodeType::CONFIGURATION_NODE), name(configName) {}
    
    std::string toHTML() const override;
};

class InfoNode : public CHTLASTNode {
public:
    std::unordered_map<std::string, std::string> info;
    
    InfoNode() : CHTLASTNode(CHTLASTNodeType::INFO_NODE) {}
    
    std::string toHTML() const override;
};

class ExportNode : public CHTLASTNode {
public:
    std::vector<std::string> exports;
    
    ExportNode() : CHTLASTNode(CHTLASTNodeType::EXPORT_NODE) {}
    
    std::string toHTML() const override;
};

} // namespace CHTL

#endif // CHTL_AST_NODE_H