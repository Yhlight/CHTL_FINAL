#include "CHTLASTNode.h"
#include <sstream>

namespace CHTL {

// CHTLASTNode基类实现
CHTLASTNode::CHTLASTNode(NodeType type) : type(type) {
}

CHTLASTNode::NodeType CHTLASTNode::getType() const {
    return type;
}

// ProgramNode实现
ProgramNode::ProgramNode() : CHTLASTNode(NodeType::PROGRAM) {
}

void ProgramNode::addStatement(std::shared_ptr<CHTLASTNode> statement) {
    statements.push_back(statement);
}

std::vector<std::shared_ptr<CHTLASTNode>> ProgramNode::getStatements() const {
    return statements;
}

std::string ProgramNode::toString() const {
    std::ostringstream oss;
    oss << "ProgramNode(" << statements.size() << " statements)";
    return oss.str();
}

std::string ProgramNode::generateCode() const {
    std::ostringstream oss;
    for (const auto& statement : statements) {
        oss << statement->generateCode() << "\n";
    }
    return oss.str();
}

// ElementNode实现
ElementNode::ElementNode(const std::string& tagName) 
    : CHTLASTNode(NodeType::ELEMENT), tagName(tagName) {
}

void ElementNode::setTagName(const std::string& tagName) {
    this->tagName = tagName;
}

std::string ElementNode::getTagName() const {
    return tagName;
}

void ElementNode::addAttribute(std::shared_ptr<CHTLASTNode> attribute) {
    attributes.push_back(attribute);
}

std::vector<std::shared_ptr<CHTLASTNode>> ElementNode::getAttributes() const {
    return attributes;
}

void ElementNode::addChild(std::shared_ptr<CHTLASTNode> child) {
    children.push_back(child);
}

std::vector<std::shared_ptr<CHTLASTNode>> ElementNode::getChildren() const {
    return children;
}

void ElementNode::setStyleBlock(std::shared_ptr<CHTLASTNode> styleBlock) {
    this->styleBlock = styleBlock;
}

std::shared_ptr<CHTLASTNode> ElementNode::getStyleBlock() const {
    return styleBlock;
}

void ElementNode::setScriptBlock(std::shared_ptr<CHTLASTNode> scriptBlock) {
    this->scriptBlock = scriptBlock;
}

std::shared_ptr<CHTLASTNode> ElementNode::getScriptBlock() const {
    return scriptBlock;
}

std::string ElementNode::toString() const {
    std::ostringstream oss;
    oss << "ElementNode(" << tagName << ", " << attributes.size() 
        << " attributes, " << children.size() << " children)";
    return oss.str();
}

std::string ElementNode::generateCode() const {
    std::ostringstream oss;
    oss << "<" << tagName;
    
    // 生成属性
    for (const auto& attr : attributes) {
        oss << " " << attr->generateCode();
    }
    
    // 检查是否是自闭合标签
    std::set<std::string> selfClosingTags = {
        "br", "hr", "img", "input", "meta", "link", "area", "base", "col", 
        "embed", "source", "track", "wbr"
    };
    
    if (selfClosingTags.find(tagName) != selfClosingTags.end()) {
        oss << " />";
    } else {
        oss << ">";
        
        // 生成子元素
        for (const auto& child : children) {
            oss << child->generateCode();
        }
        
        // 生成样式块
        if (styleBlock) {
            oss << styleBlock->generateCode();
        }
        
        // 生成脚本块
        if (scriptBlock) {
            oss << scriptBlock->generateCode();
        }
        
        oss << "</" << tagName << ">";
    }
    
    return oss.str();
}

// TextNode实现
TextNode::TextNode(const std::string& content) 
    : CHTLASTNode(NodeType::TEXT), content(content) {
}

void TextNode::setContent(const std::string& content) {
    this->content = content;
}

std::string TextNode::getContent() const {
    return content;
}

std::string TextNode::toString() const {
    return "TextNode(\"" + content + "\")";
}

std::string TextNode::generateCode() const {
    return content;
}

// AttributeNode实现
AttributeNode::AttributeNode(const std::string& name, std::shared_ptr<CHTLASTNode> value)
    : CHTLASTNode(NodeType::ATTRIBUTE), name(name), value(value) {
}

void AttributeNode::setName(const std::string& name) {
    this->name = name;
}

std::string AttributeNode::getName() const {
    return name;
}

void AttributeNode::setValue(std::shared_ptr<CHTLASTNode> value) {
    this->value = value;
}

std::shared_ptr<CHTLASTNode> AttributeNode::getValue() const {
    return value;
}

std::string AttributeNode::toString() const {
    return "AttributeNode(" + name + ")";
}

std::string AttributeNode::generateCode() const {
    if (value) {
        return name + "=\"" + value->generateCode() + "\"";
    }
    return name;
}

// StyleBlockNode实现
StyleBlockNode::StyleBlockNode() : CHTLASTNode(NodeType::STYLE_BLOCK), inline_(false) {
}

void StyleBlockNode::addRule(std::shared_ptr<CHTLASTNode> rule) {
    rules.push_back(rule);
}

std::vector<std::shared_ptr<CHTLASTNode>> StyleBlockNode::getRules() const {
    return rules;
}

void StyleBlockNode::addProperty(std::shared_ptr<CHTLASTNode> property) {
    properties.push_back(property);
}

std::vector<std::shared_ptr<CHTLASTNode>> StyleBlockNode::getProperties() const {
    return properties;
}

void StyleBlockNode::setInline(bool inline_) {
    this->inline_ = inline_;
}

bool StyleBlockNode::isInline() const {
    return inline_;
}

std::string StyleBlockNode::toString() const {
    std::ostringstream oss;
    oss << "StyleBlockNode(" << rules.size() << " rules, " 
        << properties.size() << " properties, inline: " << inline_ << ")";
    return oss.str();
}

std::string StyleBlockNode::generateCode() const {
    std::ostringstream oss;
    
    if (inline_) {
        // 内联样式
        oss << " style=\"";
        for (const auto& prop : properties) {
            oss << prop->generateCode() << "; ";
        }
        oss << "\"";
    } else {
        // 样式块
        oss << "<style>\n";
        for (const auto& rule : rules) {
            oss << rule->generateCode() << "\n";
        }
        oss << "</style>";
    }
    
    return oss.str();
}

// StyleRuleNode实现
StyleRuleNode::StyleRuleNode(std::shared_ptr<CHTLASTNode> selector)
    : CHTLASTNode(NodeType::STYLE_RULE), selector(selector) {
}

void StyleRuleNode::setSelector(std::shared_ptr<CHTLASTNode> selector) {
    this->selector = selector;
}

std::shared_ptr<CHTLASTNode> StyleRuleNode::getSelector() const {
    return selector;
}

void StyleRuleNode::addProperty(std::shared_ptr<CHTLASTNode> property) {
    properties.push_back(property);
}

std::vector<std::shared_ptr<CHTLASTNode>> StyleRuleNode::getProperties() const {
    return properties;
}

std::string StyleRuleNode::toString() const {
    return "StyleRuleNode(" + std::to_string(properties.size()) + " properties)";
}

std::string StyleRuleNode::generateCode() const {
    std::ostringstream oss;
    
    if (selector) {
        oss << selector->generateCode() << " {\n";
    }
    
    for (const auto& prop : properties) {
        oss << "  " << prop->generateCode() << ";\n";
    }
    
    if (selector) {
        oss << "}";
    }
    
    return oss.str();
}

// StyleSelectorNode实现
StyleSelectorNode::StyleSelectorNode(const std::string& selector)
    : CHTLASTNode(NodeType::STYLE_SELECTOR), selector(selector) {
}

void StyleSelectorNode::setSelector(const std::string& selector) {
    this->selector = selector;
}

std::string StyleSelectorNode::getSelector() const {
    return selector;
}

std::string StyleSelectorNode::toString() const {
    return "StyleSelectorNode(" + selector + ")";
}

std::string StyleSelectorNode::generateCode() const {
    return selector;
}

// StylePropertyNode实现
StylePropertyNode::StylePropertyNode(const std::string& name, std::shared_ptr<CHTLASTNode> value)
    : CHTLASTNode(NodeType::STYLE_PROPERTY), name(name), value(value) {
}

void StylePropertyNode::setName(const std::string& name) {
    this->name = name;
}

std::string StylePropertyNode::getName() const {
    return name;
}

void StylePropertyNode::setValue(std::shared_ptr<CHTLASTNode> value) {
    this->value = value;
}

std::shared_ptr<CHTLASTNode> StylePropertyNode::getValue() const {
    return value;
}

std::string StylePropertyNode::toString() const {
    return "StylePropertyNode(" + name + ")";
}

std::string StylePropertyNode::generateCode() const {
    if (value) {
        return name + ": " + value->generateCode();
    }
    return name;
}

// ScriptBlockNode实现
ScriptBlockNode::ScriptBlockNode(const std::string& content)
    : CHTLASTNode(NodeType::SCRIPT_BLOCK), content(content) {
}

void ScriptBlockNode::setContent(const std::string& content) {
    this->content = content;
}

std::string ScriptBlockNode::getContent() const {
    return content;
}

std::string ScriptBlockNode::toString() const {
    return "ScriptBlockNode(" + std::to_string(content.length()) + " chars)";
}

std::string ScriptBlockNode::generateCode() const {
    return "<script>\n" + content + "\n</script>";
}

// TemplateNode基类实现
TemplateNode::TemplateNode(NodeType type, const std::string& name)
    : CHTLASTNode(type), name(name) {
}

void TemplateNode::setName(const std::string& name) {
    this->name = name;
}

std::string TemplateNode::getName() const {
    return name;
}

void TemplateNode::addProperty(std::shared_ptr<CHTLASTNode> property) {
    properties.push_back(property);
}

std::vector<std::shared_ptr<CHTLASTNode>> TemplateNode::getProperties() const {
    return properties;
}

void TemplateNode::addChild(std::shared_ptr<CHTLASTNode> child) {
    children.push_back(child);
}

std::vector<std::shared_ptr<CHTLASTNode>> TemplateNode::getChildren() const {
    return children;
}

// TemplateStyleNode实现
TemplateStyleNode::TemplateStyleNode(const std::string& name)
    : TemplateNode(NodeType::TEMPLATE_STYLE, name) {
}

std::string TemplateStyleNode::toString() const {
    return "TemplateStyleNode(" + name + ")";
}

std::string TemplateStyleNode::generateCode() const {
    std::ostringstream oss;
    oss << "[Template] @Style " << name << " {\n";
    for (const auto& prop : properties) {
        oss << "  " << prop->generateCode() << ";\n";
    }
    oss << "}";
    return oss.str();
}

// TemplateElementNode实现
TemplateElementNode::TemplateElementNode(const std::string& name)
    : TemplateNode(NodeType::TEMPLATE_ELEMENT, name) {
}

std::string TemplateElementNode::toString() const {
    return "TemplateElementNode(" + name + ")";
}

std::string TemplateElementNode::generateCode() const {
    std::ostringstream oss;
    oss << "[Template] @Element " << name << " {\n";
    for (const auto& child : children) {
        oss << "  " << child->generateCode() << "\n";
    }
    oss << "}";
    return oss.str();
}

// TemplateVarNode实现
TemplateVarNode::TemplateVarNode(const std::string& name)
    : TemplateNode(NodeType::TEMPLATE_VAR, name) {
}

std::string TemplateVarNode::toString() const {
    return "TemplateVarNode(" + name + ")";
}

std::string TemplateVarNode::generateCode() const {
    std::ostringstream oss;
    oss << "[Template] @Var " << name << " {\n";
    for (const auto& prop : properties) {
        oss << "  " << prop->generateCode() << ";\n";
    }
    oss << "}";
    return oss.str();
}

// CustomNode基类实现
CustomNode::CustomNode(NodeType type, const std::string& name)
    : CHTLASTNode(type), name(name) {
}

void CustomNode::setName(const std::string& name) {
    this->name = name;
}

std::string CustomNode::getName() const {
    return name;
}

void CustomNode::addProperty(std::shared_ptr<CHTLASTNode> property) {
    properties.push_back(property);
}

std::vector<std::shared_ptr<CHTLASTNode>> CustomNode::getProperties() const {
    return properties;
}

void CustomNode::addChild(std::shared_ptr<CHTLASTNode> child) {
    children.push_back(child);
}

std::vector<std::shared_ptr<CHTLASTNode>> CustomNode::getChildren() const {
    return children;
}

// CustomStyleNode实现
CustomStyleNode::CustomStyleNode(const std::string& name)
    : CustomNode(NodeType::CUSTOM_STYLE, name) {
}

std::string CustomStyleNode::toString() const {
    return "CustomStyleNode(" + name + ")";
}

std::string CustomStyleNode::generateCode() const {
    std::ostringstream oss;
    oss << "[Custom] @Style " << name << " {\n";
    for (const auto& prop : properties) {
        oss << "  " << prop->generateCode() << ";\n";
    }
    oss << "}";
    return oss.str();
}

// CustomElementNode实现
CustomElementNode::CustomElementNode(const std::string& name)
    : CustomNode(NodeType::CUSTOM_ELEMENT, name) {
}

std::string CustomElementNode::toString() const {
    return "CustomElementNode(" + name + ")";
}

std::string CustomElementNode::generateCode() const {
    std::ostringstream oss;
    oss << "[Custom] @Element " << name << " {\n";
    for (const auto& child : children) {
        oss << "  " << child->generateCode() << "\n";
    }
    oss << "}";
    return oss.str();
}

// CustomVarNode实现
CustomVarNode::CustomVarNode(const std::string& name)
    : CustomNode(NodeType::CUSTOM_VAR, name) {
}

std::string CustomVarNode::toString() const {
    return "CustomVarNode(" + name + ")";
}

std::string CustomVarNode::generateCode() const {
    std::ostringstream oss;
    oss << "[Custom] @Var " << name << " {\n";
    for (const auto& prop : properties) {
        oss << "  " << prop->generateCode() << ";\n";
    }
    oss << "}";
    return oss.str();
}

// OriginNode实现
OriginNode::OriginNode(NodeType type, const std::string& name, const std::string& content)
    : CHTLASTNode(type), name(name), content(content) {
}

void OriginNode::setName(const std::string& name) {
    this->name = name;
}

std::string OriginNode::getName() const {
    return name;
}

void OriginNode::setContent(const std::string& content) {
    this->content = content;
}

std::string OriginNode::getContent() const {
    return content;
}

std::string OriginNode::toString() const {
    return "OriginNode(" + name + ", " + std::to_string(content.length()) + " chars)";
}

std::string OriginNode::generateCode() const {
    return content;
}

// ImportNode实现
ImportNode::ImportNode(NodeType type, const std::string& name, const std::string& path, const std::string& alias)
    : CHTLASTNode(type), name(name), path(path), alias(alias) {
}

void ImportNode::setName(const std::string& name) {
    this->name = name;
}

std::string ImportNode::getName() const {
    return name;
}

void ImportNode::setPath(const std::string& path) {
    this->path = path;
}

std::string ImportNode::getPath() const {
    return path;
}

void ImportNode::setAlias(const std::string& alias) {
    this->alias = alias;
}

std::string ImportNode::getAlias() const {
    return alias;
}

std::string ImportNode::toString() const {
    return "ImportNode(" + name + ", " + path + ", " + alias + ")";
}

std::string ImportNode::generateCode() const {
    std::ostringstream oss;
    oss << "[Import] ";
    
    switch (type) {
        case NodeType::IMPORT_HTML:
            oss << "@Html";
            break;
        case NodeType::IMPORT_STYLE:
            oss << "@Style";
            break;
        case NodeType::IMPORT_JAVASCRIPT:
            oss << "@JavaScript";
            break;
        case NodeType::IMPORT_CHTL:
            oss << "@Chtl";
            break;
        case NodeType::IMPORT_CJMOD:
            oss << "@CJmod";
            break;
        default:
            break;
    }
    
    if (!name.empty()) {
        oss << " " << name;
    }
    
    oss << " from " << path;
    
    if (!alias.empty()) {
        oss << " as " << alias;
    }
    
    return oss.str();
}

// ConfigurationNode实现
ConfigurationNode::ConfigurationNode(const std::string& name)
    : CHTLASTNode(NodeType::CONFIGURATION), name(name) {
}

void ConfigurationNode::setName(const std::string& name) {
    this->name = name;
}

std::string ConfigurationNode::getName() const {
    return name;
}

void ConfigurationNode::addProperty(std::shared_ptr<CHTLASTNode> property) {
    properties.push_back(property);
}

std::vector<std::shared_ptr<CHTLASTNode>> ConfigurationNode::getProperties() const {
    return properties;
}

void ConfigurationNode::addNameGroup(std::shared_ptr<CHTLASTNode> nameGroup) {
    this->nameGroup = nameGroup;
}

std::shared_ptr<CHTLASTNode> ConfigurationNode::getNameGroup() const {
    return nameGroup;
}

std::string ConfigurationNode::toString() const {
    return "ConfigurationNode(" + name + ", " + std::to_string(properties.size()) + " properties)";
}

std::string ConfigurationNode::generateCode() const {
    std::ostringstream oss;
    oss << "[Configuration]";
    if (!name.empty()) {
        oss << " @Config " << name;
    }
    oss << " {\n";
    
    for (const auto& prop : properties) {
        oss << "  " << prop->generateCode() << ";\n";
    }
    
    if (nameGroup) {
        oss << "  " << nameGroup->generateCode() << "\n";
    }
    
    oss << "}";
    return oss.str();
}

// ConfigPropertyNode实现
ConfigPropertyNode::ConfigPropertyNode(const std::string& name, std::shared_ptr<CHTLASTNode> value)
    : CHTLASTNode(NodeType::CONFIG_PROPERTY), name(name), value(value) {
}

void ConfigPropertyNode::setName(const std::string& name) {
    this->name = name;
}

std::string ConfigPropertyNode::getName() const {
    return name;
}

void ConfigPropertyNode::setValue(std::shared_ptr<CHTLASTNode> value) {
    this->value = value;
}

std::shared_ptr<CHTLASTNode> ConfigPropertyNode::getValue() const {
    return value;
}

std::string ConfigPropertyNode::toString() const {
    return "ConfigPropertyNode(" + name + ")";
}

std::string ConfigPropertyNode::generateCode() const {
    if (value) {
        return name + " = " + value->generateCode();
    }
    return name;
}

// NamespaceNode实现
NamespaceNode::NamespaceNode(const std::string& name)
    : CHTLASTNode(NodeType::NAMESPACE), name(name) {
}

void NamespaceNode::setName(const std::string& name) {
    this->name = name;
}

std::string NamespaceNode::getName() const {
    return name;
}

void NamespaceNode::addChild(std::shared_ptr<CHTLASTNode> child) {
    children.push_back(child);
}

std::vector<std::shared_ptr<CHTLASTNode>> NamespaceNode::getChildren() const {
    return children;
}

std::string NamespaceNode::toString() const {
    return "NamespaceNode(" + name + ", " + std::to_string(children.size()) + " children)";
}

std::string NamespaceNode::generateCode() const {
    std::ostringstream oss;
    oss << "[Namespace] " << name << " {\n";
    for (const auto& child : children) {
        oss << "  " << child->generateCode() << "\n";
    }
    oss << "}";
    return oss.str();
}

// InfoNode实现
InfoNode::InfoNode() : CHTLASTNode(NodeType::INFO) {
}

void InfoNode::addProperty(std::shared_ptr<CHTLASTNode> property) {
    properties.push_back(property);
}

std::vector<std::shared_ptr<CHTLASTNode>> InfoNode::getProperties() const {
    return properties;
}

std::string InfoNode::toString() const {
    return "InfoNode(" + std::to_string(properties.size()) + " properties)";
}

std::string InfoNode::generateCode() const {
    std::ostringstream oss;
    oss << "[Info] {\n";
    for (const auto& prop : properties) {
        oss << "  " << prop->generateCode() << ";\n";
    }
    oss << "}";
    return oss.str();
}

// ExportNode实现
ExportNode::ExportNode() : CHTLASTNode(NodeType::EXPORT) {
}

void ExportNode::addExport(std::shared_ptr<CHTLASTNode> export_) {
    exports.push_back(export_);
}

std::vector<std::shared_ptr<CHTLASTNode>> ExportNode::getExports() const {
    return exports;
}

std::string ExportNode::toString() const {
    return "ExportNode(" + std::to_string(exports.size()) + " exports)";
}

std::string ExportNode::generateCode() const {
    std::ostringstream oss;
    oss << "[Export] {\n";
    for (const auto& export_ : exports) {
        oss << "  " << export_->generateCode() << ";\n";
    }
    oss << "}";
    return oss.str();
}

// LiteralNode实现
LiteralNode::LiteralNode(const std::string& value, const std::string& type)
    : CHTLASTNode(NodeType::LITERAL), value(value), literalType(type) {
}

void LiteralNode::setValue(const std::string& value) {
    this->value = value;
}

std::string LiteralNode::getValue() const {
    return value;
}

void LiteralNode::setLiteralType(const std::string& type) {
    this->literalType = type;
}

std::string LiteralNode::getLiteralType() const {
    return literalType;
}

std::string LiteralNode::toString() const {
    return "LiteralNode(" + value + ", " + literalType + ")";
}

std::string LiteralNode::generateCode() const {
    if (literalType == "string") {
        return "\"" + value + "\"";
    }
    return value;
}

// IdentifierNode实现
IdentifierNode::IdentifierNode(const std::string& name)
    : CHTLASTNode(NodeType::IDENTIFIER), name(name) {
}

void IdentifierNode::setName(const std::string& name) {
    this->name = name;
}

std::string IdentifierNode::getName() const {
    return name;
}

std::string IdentifierNode::toString() const {
    return "IdentifierNode(" + name + ")";
}

std::string IdentifierNode::generateCode() const {
    return name;
}

// ExpressionNode实现
ExpressionNode::ExpressionNode(std::shared_ptr<CHTLASTNode> expression)
    : CHTLASTNode(NodeType::EXPRESSION), expression(expression) {
}

void ExpressionNode::setExpression(std::shared_ptr<CHTLASTNode> expression) {
    this->expression = expression;
}

std::shared_ptr<CHTLASTNode> ExpressionNode::getExpression() const {
    return expression;
}

std::string ExpressionNode::toString() const {
    return "ExpressionNode";
}

std::string ExpressionNode::generateCode() const {
    if (expression) {
        return expression->generateCode();
    }
    return "";
}

// BinaryExpressionNode实现
BinaryExpressionNode::BinaryExpressionNode(std::shared_ptr<CHTLASTNode> left, const std::string& operator_, std::shared_ptr<CHTLASTNode> right)
    : CHTLASTNode(NodeType::BINARY_EXPRESSION), left(left), operator_(operator_), right(right) {
}

void BinaryExpressionNode::setLeft(std::shared_ptr<CHTLASTNode> left) {
    this->left = left;
}

std::shared_ptr<CHTLASTNode> BinaryExpressionNode::getLeft() const {
    return left;
}

void BinaryExpressionNode::setOperator(const std::string& operator_) {
    this->operator_ = operator_;
}

std::string BinaryExpressionNode::getOperator() const {
    return operator_;
}

void BinaryExpressionNode::setRight(std::shared_ptr<CHTLASTNode> right) {
    this->right = right;
}

std::shared_ptr<CHTLASTNode> BinaryExpressionNode::getRight() const {
    return right;
}

std::string BinaryExpressionNode::toString() const {
    return "BinaryExpressionNode(" + operator_ + ")";
}

std::string BinaryExpressionNode::generateCode() const {
    if (left && right) {
        return left->generateCode() + " " + operator_ + " " + right->generateCode();
    }
    return "";
}

// ConditionalExpressionNode实现
ConditionalExpressionNode::ConditionalExpressionNode(std::shared_ptr<CHTLASTNode> condition, 
                                                   std::shared_ptr<CHTLASTNode> trueExpr, 
                                                   std::shared_ptr<CHTLASTNode> falseExpr)
    : CHTLASTNode(NodeType::CONDITIONAL_EXPRESSION), condition(condition), trueExpr(trueExpr), falseExpr(falseExpr) {
}

void ConditionalExpressionNode::setCondition(std::shared_ptr<CHTLASTNode> condition) {
    this->condition = condition;
}

std::shared_ptr<CHTLASTNode> ConditionalExpressionNode::getCondition() const {
    return condition;
}

void ConditionalExpressionNode::setTrueExpression(std::shared_ptr<CHTLASTNode> trueExpr) {
    this->trueExpr = trueExpr;
}

std::shared_ptr<CHTLASTNode> ConditionalExpressionNode::getTrueExpression() const {
    return trueExpr;
}

void ConditionalExpressionNode::setFalseExpression(std::shared_ptr<CHTLASTNode> falseExpr) {
    this->falseExpr = falseExpr;
}

std::shared_ptr<CHTLASTNode> ConditionalExpressionNode::getFalseExpression() const {
    return falseExpr;
}

std::string ConditionalExpressionNode::toString() const {
    return "ConditionalExpressionNode";
}

std::string ConditionalExpressionNode::generateCode() const {
    if (condition && trueExpr && falseExpr) {
        return condition->generateCode() + " ? " + trueExpr->generateCode() + " : " + falseExpr->generateCode();
    }
    return "";
}

// PropertyReferenceNode实现
PropertyReferenceNode::PropertyReferenceNode(const std::string& selector, const std::string& property)
    : CHTLASTNode(NodeType::PROPERTY_REFERENCE), selector(selector), property(property) {
}

void PropertyReferenceNode::setSelector(const std::string& selector) {
    this->selector = selector;
}

std::string PropertyReferenceNode::getSelector() const {
    return selector;
}

void PropertyReferenceNode::setProperty(const std::string& property) {
    this->property = property;
}

std::string PropertyReferenceNode::getProperty() const {
    return property;
}

std::string PropertyReferenceNode::toString() const {
    return "PropertyReferenceNode(" + selector + "." + property + ")";
}

std::string PropertyReferenceNode::generateCode() const {
    return selector + "." + property;
}

// FunctionCallNode实现
FunctionCallNode::FunctionCallNode(const std::string& functionName)
    : CHTLASTNode(NodeType::FUNCTION_CALL), functionName(functionName) {
}

void FunctionCallNode::setFunctionName(const std::string& functionName) {
    this->functionName = functionName;
}

std::string FunctionCallNode::getFunctionName() const {
    return functionName;
}

void FunctionCallNode::addArgument(std::shared_ptr<CHTLASTNode> argument) {
    arguments.push_back(argument);
}

std::vector<std::shared_ptr<CHTLASTNode>> FunctionCallNode::getArguments() const {
    return arguments;
}

std::string FunctionCallNode::toString() const {
    return "FunctionCallNode(" + functionName + ", " + std::to_string(arguments.size()) + " args)";
}

std::string FunctionCallNode::generateCode() const {
    std::ostringstream oss;
    oss << functionName << "(";
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << arguments[i]->generateCode();
    }
    oss << ")";
    return oss.str();
}

// UseStatementNode实现
UseStatementNode::UseStatementNode(const std::string& target)
    : CHTLASTNode(NodeType::USE_STATEMENT), target(target) {
}

void UseStatementNode::setTarget(const std::string& target) {
    this->target = target;
}

std::string UseStatementNode::getTarget() const {
    return target;
}

std::string UseStatementNode::toString() const {
    return "UseStatementNode(" + target + ")";
}

std::string UseStatementNode::generateCode() const {
    return "use " + target + ";";
}

// ExceptClauseNode实现
ExceptClauseNode::ExceptClauseNode() : CHTLASTNode(NodeType::EXCEPT_CLAUSE) {
}

void ExceptClauseNode::addException(std::shared_ptr<CHTLASTNode> exception) {
    exceptions.push_back(exception);
}

std::vector<std::shared_ptr<CHTLASTNode>> ExceptClauseNode::getExceptions() const {
    return exceptions;
}

std::string ExceptClauseNode::toString() const {
    return "ExceptClauseNode(" + std::to_string(exceptions.size()) + " exceptions)";
}

std::string ExceptClauseNode::generateCode() const {
    std::ostringstream oss;
    oss << "except ";
    for (size_t i = 0; i < exceptions.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << exceptions[i]->generateCode();
    }
    return oss.str();
}

} // namespace CHTL