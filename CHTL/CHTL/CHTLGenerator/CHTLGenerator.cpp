#include "CHTLGenerator.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLGenerator::CHTLGenerator() {}

CHTLGenerator::~CHTLGenerator() {}

void CHTLGenerator::addError(const std::string& message) {
    errors.push_back(message);
}

void CHTLGenerator::addWarning(const std::string& message) {
    warnings.push_back(message);
}

std::string CHTLGenerator::generate(std::shared_ptr<CHTLNode> ast) {
    if (!ast) {
        addError("AST为空");
        return "";
    }
    
    reset();
    
    std::string html;
    
    // 生成HTML内容
    html = generateHTML(ast);
    
    // 添加全局样式
    if (!globalStyles.empty()) {
        html = "<style>" + globalStyles + "</style>\n" + html;
    }
    
    // 添加全局脚本
    if (!globalScripts.empty()) {
        html = html + "\n<script>" + globalScripts + "</script>";
    }
    
    return html;
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<CHTLNode> node) {
    if (!node) return "";
    
    switch (node->type) {
        case CHTLNode::NodeType::ELEMENT:
            return generateElement(std::static_pointer_cast<ElementNode>(node));
        case CHTLNode::NodeType::TEXT:
            return generateText(std::static_pointer_cast<TextNode>(node));
        case CHTLNode::NodeType::COMMENT:
            return generateComment(std::static_pointer_cast<CommentNode>(node));
        case CHTLNode::NodeType::STYLE:
            return generateStyle(std::static_pointer_cast<StyleNode>(node));
        case CHTLNode::NodeType::SCRIPT:
            return generateScript(std::static_pointer_cast<ScriptNode>(node));
        case CHTLNode::NodeType::TEMPLATE:
            return generateTemplate(std::static_pointer_cast<TemplateNode>(node));
        case CHTLNode::NodeType::CUSTOM:
            return generateCustom(std::static_pointer_cast<CustomNode>(node));
        case CHTLNode::NodeType::ORIGIN:
            return generateOrigin(std::static_pointer_cast<OriginNode>(node));
        case CHTLNode::NodeType::IMPORT:
            return generateImport(std::static_pointer_cast<ImportNode>(node));
        case CHTLNode::NodeType::NAMESPACE:
            return generateNamespace(std::static_pointer_cast<NamespaceNode>(node));
        case CHTLNode::NodeType::CONSTRAINT:
            return generateConstraint(std::static_pointer_cast<ConstraintNode>(node));
        case CHTLNode::NodeType::CONFIGURATION:
            return generateConfiguration(std::static_pointer_cast<ConfigurationNode>(node));
        case CHTLNode::NodeType::USE:
            return generateUse(std::static_pointer_cast<UseNode>(node));
        default:
            addError("未知的节点类型");
            return "";
    }
}

std::string CHTLGenerator::generateElement(std::shared_ptr<ElementNode> element) {
    if (!element) return "";
    
    std::ostringstream html;
    
    // 开始标签
    html << "<" << element->tagName;
    
    // 属性
    std::string attributes = processAttributes(element->attributes);
    if (!attributes.empty()) {
        html << " " << attributes;
    }
    
    // 内联样式
    std::string inlineStyles = processInlineStyles(element->inlineStyles);
    if (!inlineStyles.empty()) {
        html << " style=\"" << inlineStyles << "\"";
    }
    
    if (element->isSelfClosing) {
        html << " />";
    } else {
        html << ">";
        
        // 子节点
        for (const auto& child : element->children) {
            html << generateHTML(child);
        }
        
        // 结束标签
        html << "</" << element->tagName << ">";
    }
    
    return html.str();
}

std::string CHTLGenerator::generateText(std::shared_ptr<TextNode> text) {
    if (!text) return "";
    return escapeHTML(text->content);
}

std::string CHTLGenerator::generateComment(std::shared_ptr<CommentNode> comment) {
    if (!comment) return "";
    
    if (comment->isGeneratorComment) {
        return "<!-- " + comment->content + " -->";
    }
    
    return ""; // 普通注释不输出
}

std::string CHTLGenerator::generateStyle(std::shared_ptr<StyleNode> style) {
    if (!style) return "";
    
    if (style->isLocal) {
        // 局部样式，添加到全局样式
        std::string css = processStyleProperties(style->inlineStyles);
        if (!css.empty()) {
            globalStyles += css + "\n";
        }
        
        // 处理类选择器
        for (const auto& className : style->classSelectors) {
            std::string selector = "." + className;
            globalStyles += selector + " {\n";
            globalStyles += processStyleProperties(style->inlineStyles);
            globalStyles += "\n}\n";
        }
        
        // 处理ID选择器
        for (const auto& id : style->idSelectors) {
            std::string selector = "#" + id;
            globalStyles += selector + " {\n";
            globalStyles += processStyleProperties(style->inlineStyles);
            globalStyles += "\n}\n";
        }
        
        return ""; // 局部样式不直接输出HTML
    } else {
        // 全局样式
        return "<style>" + style->content + "</style>";
    }
}

std::string CHTLGenerator::generateScript(std::shared_ptr<ScriptNode> script) {
    if (!script) return "";
    
    if (script->isLocal) {
        // 局部脚本，添加到全局脚本
        globalScripts += script->content + "\n";
        return ""; // 局部脚本不直接输出HTML
    } else {
        // 全局脚本
        std::ostringstream html;
        html << "<script";
        if (!script->language.empty()) {
            html << " type=\"" << script->language << "\"";
        }
        html << ">" << script->content << "</script>";
        return html.str();
    }
}

std::string CHTLGenerator::generateTemplate(std::shared_ptr<TemplateNode> templateNode) {
    if (!templateNode) return "";
    
    // 模板节点不直接输出HTML，而是被引用时使用
    addTemplate(templateNode);
    return "";
}

std::string CHTLGenerator::generateCustom(std::shared_ptr<CustomNode> customNode) {
    if (!customNode) return "";
    
    // 自定义节点不直接输出HTML，而是被引用时使用
    addCustom(customNode);
    return "";
}

std::string CHTLGenerator::generateOrigin(std::shared_ptr<OriginNode> originNode) {
    if (!originNode) return "";
    
    return processOriginContent(originNode->content, originNode->originType);
}

std::string CHTLGenerator::generateImport(std::shared_ptr<ImportNode> importNode) {
    if (!importNode) return "";
    
    return processImportContent(importNode->path, importNode->importType);
}

std::string CHTLGenerator::generateNamespace(std::shared_ptr<NamespaceNode> namespaceNode) {
    if (!namespaceNode) return "";
    
    std::string html;
    for (const auto& child : namespaceNode->children) {
        html += generateHTML(child);
    }
    return html;
}

std::string CHTLGenerator::generateConstraint(std::shared_ptr<ConstraintNode> constraintNode) {
    if (!constraintNode) return "";
    
    // 约束节点不输出HTML
    return "";
}

std::string CHTLGenerator::generateConfiguration(std::shared_ptr<ConfigurationNode> configNode) {
    if (!configNode) return "";
    
    // 配置节点不输出HTML
    addConfiguration(configNode);
    return "";
}

std::string CHTLGenerator::generateUse(std::shared_ptr<UseNode> useNode) {
    if (!useNode) return "";
    
    return processUse(useNode->target, useNode->parameters);
}

std::string CHTLGenerator::processInlineStyles(const std::map<std::string, std::string>& styles) {
    std::ostringstream css;
    bool first = true;
    
    for (const auto& style : styles) {
        if (!first) css << "; ";
        css << style.first << ": " << style.second;
        first = false;
    }
    
    return css.str();
}

std::string CHTLGenerator::processClassSelectors(const std::vector<std::string>& classes) {
    std::ostringstream css;
    
    for (const auto& className : classes) {
        css << "." << className << " {\n";
        css << "}\n";
    }
    
    return css.str();
}

std::string CHTLGenerator::processIdSelectors(const std::vector<std::string>& ids) {
    std::ostringstream css;
    
    for (const auto& id : ids) {
        css << "#" << id << " {\n";
        css << "}\n";
    }
    
    return css.str();
}

std::string CHTLGenerator::processStyleProperties(const std::map<std::string, std::string>& properties) {
    std::ostringstream css;
    bool first = true;
    
    for (const auto& prop : properties) {
        if (!first) css << "\n";
        css << "    " << prop.first << ": " << prop.second << ";";
        first = false;
    }
    
    return css.str();
}

std::string CHTLGenerator::processAttributes(const std::map<std::string, std::string>& attributes) {
    std::ostringstream attrs;
    bool first = true;
    
    for (const auto& attr : attributes) {
        if (!first) attrs << " ";
        attrs << attr.first << "=\"" << escapeHTML(attr.second) << "\"";
        first = false;
    }
    
    return attrs.str();
}

std::string CHTLGenerator::processAttributeValue(const std::string& value) {
    return escapeHTML(value);
}

std::string CHTLGenerator::processExpression(const std::string& expression) {
    // 简单的表达式处理
    return expression;
}

std::string CHTLGenerator::processConditionalExpression(const std::string& expression) {
    // 条件表达式处理
    return expression;
}

std::string CHTLGenerator::processArithmeticExpression(const std::string& expression) {
    // 算术表达式处理
    return expression;
}

std::string CHTLGenerator::processLogicalExpression(const std::string& expression) {
    // 逻辑表达式处理
    return expression;
}

std::string CHTLGenerator::processTemplateReference(const std::string& templateName, 
                                                   const std::map<std::string, std::string>& parameters) {
    auto templateNode = getTemplate(templateName);
    if (!templateNode) {
        addError("模板未找到: " + templateName);
        return "";
    }
    
    // 处理模板引用
    return "";
}

std::string CHTLGenerator::processCustomReference(const std::string& customName, 
                                                 const std::map<std::string, std::string>& parameters) {
    auto customNode = getCustom(customName);
    if (!customNode) {
        addError("自定义未找到: " + customName);
        return "";
    }
    
    // 处理自定义引用
    return "";
}

std::string CHTLGenerator::processOriginContent(const std::string& content, OriginNode::OriginType type) {
    switch (type) {
        case OriginNode::OriginType::HTML:
            return content;
        case OriginNode::OriginType::CSS:
            return "<style>" + content + "</style>";
        case OriginNode::OriginType::JAVASCRIPT:
            return "<script>" + content + "</script>";
        case OriginNode::OriginType::TEXT:
            return escapeHTML(content);
        default:
            return content;
    }
}

std::string CHTLGenerator::processImportContent(const std::string& path, ImportNode::ImportType type) {
    // 导入内容处理
    return "";
}

bool CHTLGenerator::processConstraint(const std::string& target, const std::string& condition) {
    // 约束处理
    return true;
}

std::string CHTLGenerator::processConfiguration(const std::string& configName) {
    auto config = getConfiguration(configName);
    if (!config) {
        addError("配置未找到: " + configName);
        return "";
    }
    
    return "";
}

std::string CHTLGenerator::processUse(const std::string& target, const std::vector<std::string>& parameters) {
    // Use处理
    return "";
}

std::string CHTLGenerator::escapeHTML(const std::string& text) {
    std::string result = text;
    
    // 替换HTML特殊字符
    std::regex lt("&");
    result = std::regex_replace(result, lt, "&amp;");
    
    std::regex gt("<");
    result = std::regex_replace(result, gt, "&lt;");
    
    std::regex amp(">");
    result = std::regex_replace(result, amp, "&gt;");
    
    std::regex quote("\"");
    result = std::regex_replace(result, quote, "&quot;");
    
    std::regex apos("'");
    result = std::regex_replace(result, apos, "&#39;");
    
    return result;
}

std::string CHTLGenerator::escapeCSS(const std::string& text) {
    // CSS转义
    return text;
}

std::string CHTLGenerator::escapeJavaScript(const std::string& text) {
    // JavaScript转义
    return text;
}

std::string CHTLGenerator::generateClassName(const std::string& base) {
    return base + "_" + std::to_string(std::hash<std::string>{}(base));
}

std::string CHTLGenerator::generateId(const std::string& base) {
    return base + "_" + std::to_string(std::hash<std::string>{}(base));
}

std::string CHTLGenerator::generateCSS() {
    return globalStyles;
}

std::string CHTLGenerator::generateJavaScript() {
    return globalScripts;
}

std::string CHTLGenerator::generateComplete() {
    std::ostringstream html;
    
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "<meta charset=\"UTF-8\">\n";
    
    if (!globalStyles.empty()) {
        html << "<style>\n" << globalStyles << "</style>\n";
    }
    
    html << "</head>\n";
    html << "<body>\n";
    html << "</body>\n";
    
    if (!globalScripts.empty()) {
        html << "<script>\n" << globalScripts << "</script>\n";
    }
    
    html << "</html>";
    
    return html.str();
}

void CHTLGenerator::addTemplate(std::shared_ptr<TemplateNode> templateNode) {
    if (templateNode) {
        templates[templateNode->name] = templateNode;
    }
}

void CHTLGenerator::addCustom(std::shared_ptr<CustomNode> customNode) {
    if (customNode) {
        customs[customNode->name] = customNode;
    }
}

void CHTLGenerator::addConfiguration(std::shared_ptr<ConfigurationNode> configNode) {
    if (configNode) {
        configurations[configNode->name] = configNode;
    }
}

std::shared_ptr<TemplateNode> CHTLGenerator::getTemplate(const std::string& name) const {
    auto it = templates.find(name);
    return it != templates.end() ? it->second : nullptr;
}

std::shared_ptr<CustomNode> CHTLGenerator::getCustom(const std::string& name) const {
    auto it = customs.find(name);
    return it != customs.end() ? it->second : nullptr;
}

std::shared_ptr<ConfigurationNode> CHTLGenerator::getConfiguration(const std::string& name) const {
    auto it = configurations.find(name);
    return it != configurations.end() ? it->second : nullptr;
}

bool CHTLGenerator::hasErrors() const {
    return !errors.empty();
}

std::vector<std::string> CHTLGenerator::getErrors() const {
    return errors;
}

std::vector<std::string> CHTLGenerator::getWarnings() const {
    return warnings;
}

void CHTLGenerator::printErrors() const {
    for (const auto& error : errors) {
        std::cout << "错误: " << error << std::endl;
    }
}

void CHTLGenerator::printWarnings() const {
    for (const auto& warning : warnings) {
        std::cout << "警告: " << warning << std::endl;
    }
}

void CHTLGenerator::reset() {
    errors.clear();
    warnings.clear();
    globalStyles.clear();
    globalScripts.clear();
    localStyles.clear();
    localScripts.clear();
}

} // namespace CHTL