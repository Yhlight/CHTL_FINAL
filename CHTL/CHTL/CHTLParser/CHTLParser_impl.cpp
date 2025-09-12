#include "CHTLParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

// ElementNode实现
void ElementNode::addAttribute(const std::string& name, const std::string& value) {
    attributes[name] = value;
}

void ElementNode::addChild(std::shared_ptr<CHTLNode> child) {
    children.push_back(child);
}

std::string ElementNode::getAttribute(const std::string& name) const {
    auto it = attributes.find(name);
    return it != attributes.end() ? it->second : "";
}

bool ElementNode::hasAttribute(const std::string& name) const {
    return attributes.find(name) != attributes.end();
}

std::string ElementNode::toString() const {
    std::ostringstream oss;
    oss << "ElementNode(" << tagName << ", " << children.size() << " children)";
    return oss.str();
}

std::string ElementNode::toHTML() const {
    std::ostringstream oss;
    
    // 开始标签
    oss << "<" << tagName;
    
    // 属性
    for (const auto& attr : attributes) {
        oss << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    if (isSelfClosing) {
        oss << " />";
    } else {
        oss << ">";
        
        // 子节点
        for (const auto& child : children) {
            oss << child->toHTML();
        }
        
        // 结束标签
        oss << "</" << tagName << ">";
    }
    
    return oss.str();
}

// TextNode实现
std::string TextNode::toString() const {
    std::ostringstream oss;
    oss << "TextNode(\"" << content << "\")";
    return oss.str();
}

std::string TextNode::toHTML() const {
    return content;
}

// CommentNode实现
std::string CommentNode::toString() const {
    std::ostringstream oss;
    oss << "CommentNode(\"" << content << "\", " << (isGeneratorComment ? "generator" : "normal") << ")";
    return oss.str();
}

std::string CommentNode::toHTML() const {
    if (isGeneratorComment) {
        return "<!-- " + content + " -->";
    }
    return ""; // 普通注释不输出到HTML
}

// StyleNode实现
void StyleNode::addInlineStyle(const std::string& property, const std::string& value) {
    inlineStyles[property] = value;
}

void StyleNode::addClassSelector(const std::string& className) {
    classSelectors.push_back(className);
}

void StyleNode::addIdSelector(const std::string& id) {
    idSelectors.push_back(id);
}

std::string StyleNode::toString() const {
    std::ostringstream oss;
    oss << "StyleNode(" << (isLocal ? "local" : "global") << ", " << inlineStyles.size() << " inline styles)";
    return oss.str();
}

std::string StyleNode::toHTML() const {
    if (isLocal) {
        // 局部样式块，生成内联样式
        std::ostringstream oss;
        if (!inlineStyles.empty()) {
            oss << " style=\"";
            bool first = true;
            for (const auto& style : inlineStyles) {
                if (!first) oss << "; ";
                oss << style.first << ": " << style.second;
                first = false;
            }
            oss << "\"";
        }
        return oss.str();
    } else {
        // 全局样式块
        return "<style>" + content + "</style>";
    }
}

// ScriptNode实现
std::string ScriptNode::toString() const {
    std::ostringstream oss;
    oss << "ScriptNode(" << (isLocal ? "local" : "global") << ", \"" << content << "\")";
    return oss.str();
}

std::string ScriptNode::toHTML() const {
    if (isLocal) {
        // 局部脚本，不直接输出HTML
        return "";
    } else {
        // 全局脚本
        std::ostringstream oss;
        oss << "<script";
        if (!language.empty()) {
            oss << " type=\"" << language << "\"";
        }
        oss << ">" << content << "</script>";
        return oss.str();
    }
}

// TemplateNode实现
void TemplateNode::addProperty(const std::string& key, const std::string& value) {
    properties[key] = value;
}

void TemplateNode::addChild(std::shared_ptr<CHTLNode> child) {
    children.push_back(child);
}

void TemplateNode::addInherit(const std::string& templateName) {
    inherits.push_back(templateName);
}

std::string TemplateNode::toString() const {
    std::ostringstream oss;
    oss << "TemplateNode(" << name << ", " << properties.size() << " properties)";
    return oss.str();
}

std::string TemplateNode::toHTML() const {
    // 模板节点不直接输出HTML，而是被引用时使用
    return "";
}

// CustomNode实现
void CustomNode::addProperty(const std::string& key, const std::string& value) {
    properties[key] = value;
}

void CustomNode::addChild(std::shared_ptr<CHTLNode> child) {
    children.push_back(child);
}

void CustomNode::addSpecialization(const std::string& spec) {
    specializations.push_back(spec);
}

std::string CustomNode::toString() const {
    std::ostringstream oss;
    oss << "CustomNode(" << name << ", " << properties.size() << " properties)";
    return oss.str();
}

std::string CustomNode::toHTML() const {
    // 自定义节点不直接输出HTML，而是被引用时使用
    return "";
}

// OriginNode实现
std::string OriginNode::toString() const {
    std::ostringstream oss;
    oss << "OriginNode(" << name << ", " << content.length() << " chars)";
    return oss.str();
}

std::string OriginNode::toHTML() const {
    return content;
}

// ImportNode实现
void ImportNode::addTarget(const std::string& target) {
    targets.push_back(target);
}

void ImportNode::setAlias(const std::string& alias) {
    this->alias = alias;
}

std::string ImportNode::toString() const {
    std::ostringstream oss;
    oss << "ImportNode(" << path << ", " << targets.size() << " targets)";
    return oss.str();
}

std::string ImportNode::toHTML() const {
    // 导入节点不直接输出HTML，而是被处理时使用
    return "";
}

// NamespaceNode实现
void NamespaceNode::addChild(std::shared_ptr<CHTLNode> child) {
    children.push_back(child);
}

void NamespaceNode::setParent(const std::string& parent) {
    parentNamespace = parent;
}

std::string NamespaceNode::toString() const {
    std::ostringstream oss;
    oss << "NamespaceNode(" << name << ", " << children.size() << " children)";
    return oss.str();
}

std::string NamespaceNode::toHTML() const {
    std::ostringstream oss;
    for (const auto& child : children) {
        oss << child->toHTML();
    }
    return oss.str();
}

// ConstraintNode实现
void ConstraintNode::setCondition(const std::string& cond) {
    condition = cond;
}

std::string ConstraintNode::toString() const {
    std::ostringstream oss;
    oss << "ConstraintNode(" << target << ", " << condition << ")";
    return oss.str();
}

std::string ConstraintNode::toHTML() const {
    // 约束节点不输出HTML
    return "";
}

// ConfigurationNode实现
void ConfigurationNode::addProperty(const std::string& key, const std::string& value) {
    properties[key] = value;
}

std::string ConfigurationNode::toString() const {
    std::ostringstream oss;
    oss << "ConfigurationNode(" << name << ", " << properties.size() << " properties)";
    return oss.str();
}

std::string ConfigurationNode::toHTML() const {
    // 配置节点不输出HTML
    return "";
}

// UseNode实现
void UseNode::addParameter(const std::string& param) {
    parameters.push_back(param);
}

std::string UseNode::toString() const {
    std::ostringstream oss;
    oss << "UseNode(" << target << ", " << parameters.size() << " parameters)";
    return oss.str();
}

std::string UseNode::toHTML() const {
    // Use节点不直接输出HTML
    return "";
}

// CHTLParser实现
CHTLParser::CHTLParser() : currentToken(0) {}

CHTLParser::CHTLParser(const std::vector<CHTLToken>& tokens) : tokens(tokens), currentToken(0) {}



CHTLToken CHTLParser::current() const {
    if (currentToken >= tokens.size()) {
        return CHTLToken(CHTLTokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return tokens[currentToken];
}

CHTLToken CHTLParser::peek(size_t offset) const {
    if (currentToken + offset >= tokens.size()) {
        return CHTLToken(CHTLTokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return tokens[currentToken + offset];
}

void CHTLParser::advance() {
    if (currentToken < tokens.size()) {
        currentToken++;
    }
}

bool CHTLParser::match(CHTLTokenType type) const {
    return current().type == type;
}

bool CHTLParser::match(const std::vector<CHTLTokenType>& types) const {
    CHTLTokenType currentType = current().type;
    return std::find(types.begin(), types.end(), currentType) != types.end();
}

void CHTLParser::consume(CHTLTokenType type, const std::string& errorMessage) {
    if (!match(type)) {
        addError(errorMessage);
        return;
    }
    advance();
}

void CHTLParser::skipWhitespaceAndComments() {
    while (currentToken < tokens.size()) {
        CHTLTokenType type = current().type;
        if (type == CHTLTokenType::LINE_COMMENT || 
            type == CHTLTokenType::BLOCK_COMMENT || 
            type == CHTLTokenType::GENERATOR_COMMENT) {
            advance();
        } else {
            // 在CHTL中，空白字符在词法分析阶段就被处理了
            // 这里只需要跳过注释
            break;
        }
    }
}

void CHTLParser::addError(const std::string& message) {
    std::ostringstream oss;
    oss << "第" << current().line << "行第" << current().column << "列: " << message;
    errors.push_back(oss.str());
}

void CHTLParser::addWarning(const std::string& message) {
    std::ostringstream oss;
    oss << "第" << current().line << "行第" << current().column << "列: " << message;
    warnings.push_back(oss.str());
}

CHTLToken CHTLParser::createErrorToken(const std::string& message) {
    addError(message);
    return CHTLToken(CHTLTokenType::ERROR, message, current().line, current().column, 0);
}

} // namespace CHTL