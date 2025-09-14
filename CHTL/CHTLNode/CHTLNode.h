#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

// 基础节点类
class CHTLNode {
public:
    enum class Type {
        DOCUMENT,
        ELEMENT,
        TEXT,
        COMMENT,
        TEMPLATE,
        CUSTOM,
        ORIGIN,
        IMPORT,
        NAMESPACE,
        CONFIGURATION,
        STYLE,
        SCRIPT
    };
    
    Type type;
    std::string value;
    std::vector<std::unique_ptr<CHTLNode>> children;
    std::map<std::string, std::string> attributes;
    
    CHTLNode(Type t, const std::string& v = "") : type(t), value(v) {}
    virtual ~CHTLNode() = default;
    
    virtual std::string toString() const = 0;
    virtual std::string toHTML() const = 0;
    
    void addChild(std::unique_ptr<CHTLNode> child);
    void setAttribute(const std::string& name, const std::string& value);
    std::string getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
};

// 文档节点
class DocumentNode : public CHTLNode {
public:
    DocumentNode() : CHTLNode(Type::DOCUMENT) {}
    std::string toString() const override;
    std::string toHTML() const override;
};

// 元素节点
class ElementNode : public CHTLNode {
public:
    std::string tagName;
    std::vector<Attribute> attributes;
    std::vector<std::unique_ptr<CHTLNode>> children;
    
    ElementNode(const std::string& tag) : CHTLNode(Type::ELEMENT), tagName(tag) {}
    std::string toString() const override;
    std::string toHTML() const override;
};

// 文本节点
class TextNode : public CHTLNode {
public:
    std::string text;
    bool isLiteral;
    
    TextNode(const std::string& t, bool literal = false) 
        : CHTLNode(Type::TEXT), text(t), isLiteral(literal) {}
    std::string toString() const override;
    std::string toHTML() const override;
};

// 注释节点
class CommentNode : public CHTLNode {
public:
    std::string comment;
    bool isGeneratorComment;
    
    CommentNode(const std::string& c, bool generator = false) 
        : CHTLNode(Type::COMMENT), comment(c), isGeneratorComment(generator) {}
    std::string toString() const override;
    std::string toHTML() const override;
};

// 样式节点
class StyleNode : public CHTLNode {
public:
    std::vector<StyleRule> rules;
    bool isLocal;
    
    StyleNode(bool local = false) : CHTLNode(Type::STYLE), isLocal(local) {}
    std::string toString() const override;
    std::string toHTML() const override;
};

// 脚本节点
class ScriptNode : public CHTLNode {
public:
    std::string script;
    bool isLocal;
    
    ScriptNode(const std::string& s, bool local = false) 
        : CHTLNode(Type::SCRIPT), script(s), isLocal(local) {}
    std::string toString() const override;
    std::string toHTML() const override;
};

// 模板节点
class TemplateNode : public CHTLNode {
public:
    enum class TemplateType {
        STYLE,
        ELEMENT,
        VAR
    };
    
    TemplateType templateType;
    std::string name;
    std::vector<std::unique_ptr<CHTLNode>> content;
    
    TemplateNode(TemplateType type, const std::string& n) 
        : CHTLNode(Type::TEMPLATE), templateType(type), name(n) {}
    std::string toString() const override;
    std::string toHTML() const override;
};

// 自定义节点
class CustomNode : public CHTLNode {
public:
    enum class CustomType {
        STYLE,
        ELEMENT,
        VAR
    };
    
    CustomType customType;
    std::string name;
    std::vector<std::unique_ptr<CHTLNode>> content;
    std::vector<CustomOperation> operations;
    
    CustomNode(CustomType type, const std::string& n) 
        : CHTLNode(Type::CUSTOM), customType(type), name(n) {}
    std::string toString() const override;
    std::string toHTML() const override;
};

// 原始嵌入节点
class OriginNode : public CHTLNode {
public:
    enum class OriginType {
        HTML,
        CSS,
        JAVASCRIPT,
        CUSTOM
    };
    
    OriginType originType;
    std::string name;
    std::string content;
    
    OriginNode(OriginType type, const std::string& n, const std::string& c) 
        : CHTLNode(Type::ORIGIN), originType(type), name(n), content(c) {}
    std::string toString() const override;
    std::string toHTML() const override;
};

// 导入节点
class ImportNode : public CHTLNode {
public:
    enum class ImportType {
        HTML,
        CSS,
        JAVASCRIPT,
        CHTL,
        CJMOD
    };
    
    ImportType importType;
    std::string path;
    std::string alias;
    std::string target;
    
    ImportNode(ImportType type, const std::string& p) 
        : CHTLNode(Type::IMPORT), importType(type), path(p) {}
    std::string toString() const override;
    std::string toHTML() const override;
};

// 命名空间节点
class NamespaceNode : public CHTLNode {
public:
    std::string name;
    std::vector<std::unique_ptr<CHTLNode>> content;
    
    NamespaceNode(const std::string& n) : CHTLNode(Type::NAMESPACE), name(n) {}
    std::string toString() const override;
    std::string toHTML() const override;
};

// 配置节点
class ConfigurationNode : public CHTLNode {
public:
    std::string name;
    std::map<std::string, std::string> settings;
    std::map<std::string, std::string> keywords;
    
    ConfigurationNode(const std::string& n = "") : CHTLNode(Type::CONFIGURATION), name(n) {}
    std::string toString() const override;
    std::string toHTML() const override;
};

// 辅助结构
struct Attribute {
    std::string name;
    std::string value;
    bool isLiteral;
    
    Attribute(const std::string& n, const std::string& v, bool literal = false) 
        : name(n), value(v), isLiteral(literal) {}
};

struct StyleRule {
    std::string selector;
    std::map<std::string, std::string> properties;
    std::vector<StyleRule> nestedRules;
    
    StyleRule(const std::string& s) : selector(s) {}
};

struct CustomOperation {
    enum class Type {
        DELETE,
        INSERT,
        REPLACE,
        INHERIT
    };
    
    Type type;
    std::string target;
    std::vector<std::unique_ptr<CHTLNode>> content;
    
    CustomOperation(Type t, const std::string& target) : type(t), target(target) {}
};

} // namespace CHTL