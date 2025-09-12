#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "CHTL/CHTLLexer/CHTLLexer.h"
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace CHTL {

// 前向声明
class CHTLNode;
class ElementNode;
class TextNode;
class CommentNode;
class StyleNode;
class ScriptNode;
class TemplateNode;
class CustomNode;
class OriginNode;
class ImportNode;
class NamespaceNode;
class ConstraintNode;
class ConfigurationNode;
class UseNode;

// CHTL AST节点基类
class CHTLNode {
public:
    enum class NodeType {
        ELEMENT,
        TEXT,
        COMMENT,
        STYLE,
        SCRIPT,
        TEMPLATE,
        CUSTOM,
        ORIGIN,
        IMPORT,
        NAMESPACE,
        CONSTRAINT,
        CONFIGURATION,
        USE
    };
    
    NodeType type;
    size_t line;
    size_t column;
    
    CHTLNode(NodeType t, size_t l = 0, size_t c = 0) : type(t), line(l), column(c) {}
    virtual ~CHTLNode() = default;
    
    virtual std::string toString() const = 0;
    virtual std::string toHTML() const = 0;
};

// 元素节点
class ElementNode : public CHTLNode {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::map<std::string, std::string> inlineStyles;
    std::vector<std::shared_ptr<CHTLNode>> children;
    bool isSelfClosing;
    
    ElementNode(const std::string& tag, size_t l = 0, size_t c = 0) 
        : CHTLNode(NodeType::ELEMENT, l, c), tagName(tag), isSelfClosing(false) {}
    
    void addAttribute(const std::string& name, const std::string& value);
    void addChild(std::shared_ptr<CHTLNode> child);
    std::string getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    
    std::string toString() const override;
    std::string toHTML() const override;
};

// 文本节点
class TextNode : public CHTLNode {
public:
    std::string content;
    
    TextNode(const std::string& text, size_t l = 0, size_t c = 0) 
        : CHTLNode(NodeType::TEXT, l, c), content(text) {}
    
    std::string toString() const override;
    std::string toHTML() const override;
};

// 注释节点
class CommentNode : public CHTLNode {
public:
    std::string content;
    bool isGeneratorComment;
    
    CommentNode(const std::string& text, bool isGen = false, size_t l = 0, size_t c = 0) 
        : CHTLNode(NodeType::COMMENT, l, c), content(text), isGeneratorComment(isGen) {}
    
    std::string toString() const override;
    std::string toHTML() const override;
};

// 样式节点
class StyleNode : public CHTLNode {
public:
    std::string content;
    std::map<std::string, std::string> inlineStyles;
    std::vector<std::string> classSelectors;
    std::vector<std::string> idSelectors;
    std::map<std::string, std::map<std::string, std::string>> cssRules; // selector -> properties
    std::vector<std::pair<std::string, std::string>> templateUsages; // template type -> template name
    bool isLocal;
    
    StyleNode(const std::string& css, bool local = false, size_t l = 0, size_t c = 0) 
        : CHTLNode(NodeType::STYLE, l, c), content(css), isLocal(local) {}
    
    void addInlineStyle(const std::string& property, const std::string& value);
    void addClassSelector(const std::string& className);
    void addIdSelector(const std::string& id);
    void addCSSRule(const std::string& selector, const std::string& property, const std::string& value);
    void addTemplateUsage(const std::string& templateType, const std::string& templateName);
    
    std::string toString() const override;
    std::string toHTML() const override;
};

// 脚本节点
class ScriptNode : public CHTLNode {
public:
    std::string content;
    std::string language;
    bool isLocal;
    
    ScriptNode(const std::string& js, bool local = false, size_t l = 0, size_t c = 0) 
        : CHTLNode(NodeType::SCRIPT, l, c), content(js), isLocal(local) {}
    
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
    std::map<std::string, std::string> properties;
    std::vector<std::shared_ptr<CHTLNode>> children;
    std::vector<std::string> inherits;
    
    TemplateNode(TemplateType type, const std::string& n, size_t l = 0, size_t c = 0) 
        : CHTLNode(NodeType::TEMPLATE, l, c), templateType(type), name(n) {}
    
    void addProperty(const std::string& key, const std::string& value);
    void addChild(std::shared_ptr<CHTLNode> child);
    void addInherit(const std::string& templateName);
    
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
    std::map<std::string, std::string> properties;
    std::vector<std::shared_ptr<CHTLNode>> children;
    std::vector<std::string> specializations;
    
    CustomNode(CustomType type, const std::string& n, size_t l = 0, size_t c = 0) 
        : CHTLNode(NodeType::CUSTOM, l, c), customType(type), name(n) {}
    
    void addProperty(const std::string& key, const std::string& value);
    void addChild(std::shared_ptr<CHTLNode> child);
    void addSpecialization(const std::string& spec);
    
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
        TEXT
    };
    
    OriginType originType;
    std::string content;
    std::string name;
    
    OriginNode(OriginType type, const std::string& c, size_t l = 0, size_t c_pos = 0) 
        : CHTLNode(NodeType::ORIGIN, l, c_pos), originType(type), content(c) {}
    
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
        CHTL
    };
    
    ImportType importType;
    std::string path;
    std::string alias;
    std::vector<std::string> targets;
    
    ImportNode(ImportType type, const std::string& p, size_t l = 0, size_t c = 0) 
        : CHTLNode(NodeType::IMPORT, l, c), importType(type), path(p) {}
    
    void addTarget(const std::string& target);
    void setAlias(const std::string& alias);
    
    std::string toString() const override;
    std::string toHTML() const override;
};

// 命名空间节点
class NamespaceNode : public CHTLNode {
public:
    std::string name;
    std::vector<std::shared_ptr<CHTLNode>> children;
    std::string parentNamespace;
    
    NamespaceNode(const std::string& n, size_t l = 0, size_t c = 0) 
        : CHTLNode(NodeType::NAMESPACE, l, c), name(n) {}
    
    void addChild(std::shared_ptr<CHTLNode> child);
    void setParent(const std::string& parent);
    
    std::string toString() const override;
    std::string toHTML() const override;
};

// 约束节点
class ConstraintNode : public CHTLNode {
public:
    enum class ConstraintType {
        PRECISE,
        TYPE,
        GLOBAL
    };
    
    ConstraintType constraintType;
    std::string target;
    std::string condition;
    
    ConstraintNode(ConstraintType type, const std::string& t, size_t l = 0, size_t c = 0) 
        : CHTLNode(NodeType::CONSTRAINT, l, c), constraintType(type), target(t) {}
    
    void setCondition(const std::string& cond);
    
    std::string toString() const override;
    std::string toHTML() const override;
};

// 配置节点
class ConfigurationNode : public CHTLNode {
public:
    std::string name;
    std::map<std::string, std::string> properties;
    
    ConfigurationNode(const std::string& n, size_t l = 0, size_t c = 0) 
        : CHTLNode(NodeType::CONFIGURATION, l, c), name(n) {}
    
    void addProperty(const std::string& key, const std::string& value);
    
    std::string toString() const override;
    std::string toHTML() const override;
};

// Use节点
class UseNode : public CHTLNode {
public:
    std::string target;
    std::vector<std::string> parameters;
    
    UseNode(const std::string& t, size_t l = 0, size_t c = 0) 
        : CHTLNode(NodeType::USE, l, c), target(t) {}
    
    void addParameter(const std::string& param);
    
    std::string toString() const override;
    std::string toHTML() const override;
};

// CHTL解析器
class CHTLParser {
private:
    std::vector<CHTLToken> tokens;
    size_t currentToken;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 模板和自定义存储
    std::map<std::string, std::shared_ptr<TemplateNode>> templates;
    std::map<std::string, std::shared_ptr<CustomNode>> customs;
    std::map<std::string, std::shared_ptr<ConfigurationNode>> configurations;
    
    // Token处理
    CHTLToken current() const;
    CHTLToken peek(size_t offset = 1) const;
    void advance();
    bool match(CHTLTokenType type) const;
    bool match(const std::vector<CHTLTokenType>& types) const;
    void consume(CHTLTokenType type, const std::string& errorMessage);
    void skipWhitespaceAndComments();
    
    // 错误处理
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    CHTLToken createErrorToken(const std::string& message);
    
    // 解析方法
    std::shared_ptr<CHTLNode> parseDocument();
    std::shared_ptr<CHTLNode> parseElement();
    std::shared_ptr<CHTLNode> parseText();
    std::shared_ptr<CHTLNode> parseComment();
    std::shared_ptr<CHTLNode> parseStyle();
    std::shared_ptr<CHTLNode> parseScript();
    std::shared_ptr<CHTLNode> parseTemplate();
    std::shared_ptr<CHTLNode> parseCustom();
    std::shared_ptr<CHTLNode> parseOrigin();
    std::shared_ptr<CHTLNode> parseImport();
    std::shared_ptr<CHTLNode> parseNamespace();
    std::shared_ptr<CHTLNode> parseConstraint();
    std::shared_ptr<CHTLNode> parseConfiguration();
    std::shared_ptr<CHTLNode> parseUse();
    
    // 属性解析
    std::map<std::string, std::string> parseAttributes();
    std::string parseAttributeValue();
    
    // 样式解析
    std::map<std::string, std::string> parseStyleProperties();
    std::string parseStylePropertyValue();
    void parseCSSRule(std::shared_ptr<StyleNode> style);
    void parseTemplateUsage(std::shared_ptr<StyleNode> style);
    
    // 模板解析
    TemplateNode::TemplateType parseTemplateType();
    std::shared_ptr<TemplateNode> parseStyleTemplate();
    std::shared_ptr<TemplateNode> parseElementTemplate();
    std::shared_ptr<TemplateNode> parseVariableTemplate();
    
    // 自定义解析
    CustomNode::CustomType parseCustomType();
    std::shared_ptr<CustomNode> parseCustomStyle();
    std::shared_ptr<CustomNode> parseCustomElement();
    std::shared_ptr<CustomNode> parseCustomVariable();
    
    // 原始嵌入解析
    OriginNode::OriginType parseOriginType();
    std::shared_ptr<OriginNode> parseOriginHTML();
    std::shared_ptr<OriginNode> parseOriginCSS();
    std::shared_ptr<OriginNode> parseOriginJavaScript();
    
    // 导入解析
    ImportNode::ImportType parseImportType();
    std::shared_ptr<ImportNode> parseImportHTML();
    std::shared_ptr<ImportNode> parseImportCSS();
    std::shared_ptr<ImportNode> parseImportJavaScript();
    std::shared_ptr<ImportNode> parseImportCHTL();
    
    // 约束解析
    ConstraintNode::ConstraintType parseConstraintType();
    
    // 表达式解析
    std::string parseExpression();
    std::string parseConditionalExpression();
    std::string parseArithmeticExpression();
    std::string parseLogicalExpression();
    
public:
    CHTLParser();
    explicit CHTLParser(const std::vector<CHTLToken>& tokens);
    
    // 主要方法
    std::shared_ptr<CHTLNode> parse();
    std::shared_ptr<CHTLNode> parse(const std::vector<CHTLToken>& tokens);
    
    // 状态查询
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;
    
    // 模板和自定义管理
    void addTemplate(std::shared_ptr<TemplateNode> templateNode);
    void addCustom(std::shared_ptr<CustomNode> customNode);
    void addConfiguration(std::shared_ptr<ConfigurationNode> configNode);
    
    std::shared_ptr<TemplateNode> getTemplate(const std::string& name) const;
    std::shared_ptr<CustomNode> getCustom(const std::string& name) const;
    std::shared_ptr<ConfigurationNode> getConfiguration(const std::string& name) const;
    
    // 调试
    void printAST(std::shared_ptr<CHTLNode> node, int depth = 0) const;
    void printErrors() const;
    void printWarnings() const;
    
    // 重置
    void reset();
    void setTokens(const std::vector<CHTLToken>& tokens);
};

} // namespace CHTL

#endif // CHTL_PARSER_H