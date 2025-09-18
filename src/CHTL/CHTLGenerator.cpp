#include "CHTLGenerator.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLGenerator::CHTLGenerator() {
    // 默认配置
    options_.includeDefaultStructure = false;
    options_.inlineCSS = false;
    options_.inlineJS = false;
    options_.minify = false;
    options_.debugMode = false;
    options_.outputDirectory = ".";
    options_.cssFileName = "style.css";
    options_.jsFileName = "script.js";
    options_.htmlFileName = "index.html";
}

GenerationResult CHTLGenerator::generate(NodePtr root, const GenerationOptions& options) {
    options_ = options;
    errors_.clear();
    warnings_.clear();
    
    GenerationResult result;
    
    // 验证输入
    if (!root) {
        addError("Root node is null");
        result.errors = errors_;
        return result;
    }
    
    // 处理模板、导入、配置等
    processTemplates(root);
    processImports(root);
    processConfiguration(root);
    processNamespaces(root);
    
    // 生成HTML
    result.html = generateHTMLFromNode(root);
    
    // 生成CSS
    result.css = generateCSSFromNode(root);
    
    // 生成JS
    result.js = generateJSFromNode(root);
    
    // 应用选项
    if (options_.minify) {
        result.html = minifyHTML(result.html);
        result.css = minifyCSS(result.css);
        result.js = minifyJS(result.js);
    }
    
    if (options_.includeDefaultStructure) {
        result.html = generateDefaultHTMLStructure() + result.html;
        result.css = generateDefaultCSSStructure() + result.css;
        result.js = generateDefaultJSStructure() + result.js;
    }
    
    result.errors = errors_;
    result.warnings = warnings_;
    
    return result;
}

GenerationResult CHTLGenerator::generateHTML(NodePtr root, const GenerationOptions& options) {
    options_ = options;
    errors_.clear();
    warnings_.clear();
    
    GenerationResult result;
    result.html = generateHTMLFromNode(root);
    result.errors = errors_;
    result.warnings = warnings_;
    
    return result;
}

GenerationResult CHTLGenerator::generateCSS(NodePtr root, const GenerationOptions& options) {
    options_ = options;
    errors_.clear();
    warnings_.clear();
    
    GenerationResult result;
    result.css = generateCSSFromNode(root);
    result.errors = errors_;
    result.warnings = warnings_;
    
    return result;
}

GenerationResult CHTLGenerator::generateJS(NodePtr root, const GenerationOptions& options) {
    options_ = options;
    errors_.clear();
    warnings_.clear();
    
    GenerationResult result;
    result.js = generateJSFromNode(root);
    result.errors = errors_;
    result.warnings = warnings_;
    
    return result;
}

std::string CHTLGenerator::generateHTMLFromNode(NodePtr node, int indentLevel) {
    if (!node) return "";
    
    switch (node->getType()) {
        case NodeType::ELEMENT: {
            auto element = std::dynamic_pointer_cast<ElementNode>(node);
            return generateElementHTML(element.get(), indentLevel);
        }
        case NodeType::TEXT: {
            auto textNode = std::dynamic_pointer_cast<TextNode>(node);
            return generateTextHTML(textNode.get(), indentLevel);
        }
        case NodeType::TEMPLATE:
            return generateTemplateHTML(node, indentLevel);
        case NodeType::CUSTOM:
            return generateCustomHTML(node, indentLevel);
        case NodeType::STYLE:
            return generateStyleHTML(node, indentLevel);
        case NodeType::SCRIPT:
            return generateScriptHTML(node, indentLevel);
        case NodeType::ORIGIN:
            return generateOriginHTML(node, indentLevel);
        case NodeType::IMPORT:
            return generateImportHTML(node, indentLevel);
        case NodeType::CONFIG:
            return generateConfigurationHTML(node, indentLevel);
        case NodeType::NAMESPACE:
            return generateNamespaceHTML(node, indentLevel);
        case NodeType::OPERATOR:
            return generateOperatorHTML(node, indentLevel);
        default:
            return "";
    }
}

std::string CHTLGenerator::generateElementHTML(ElementNode* element, int indentLevel) {
    if (!element) return "";
    
    std::ostringstream oss;
    std::string indent = getIndent(indentLevel);
    
    // 开始标签
    oss << indent << "<" << element->getTagName();
    
    // 属性
    std::string attrs = formatAttributes(element);
    if (!attrs.empty()) {
        oss << " " << attrs;
    }
    
    // 内联样式
    std::string styles = element->getInlineStyles();
    if (!styles.empty()) {
        oss << " style=\"" << styles << "\"";
    }
    
    // 自闭合标签
    if (element->isSelfClosing()) {
        oss << " />";
        return oss.str();
    }
    
    oss << ">";
    
    // 文本内容
    if (!element->getTextContent().empty()) {
        oss << element->getTextContent();
    }
    
    // 子节点
    for (const auto& child : element->getChildren()) {
        std::string childHTML = generateHTMLFromNode(child, indentLevel + 1);
        if (!childHTML.empty()) {
            oss << "\n" << childHTML;
        }
    }
    
    // 结束标签
    if (!element->getChildren().empty() || !element->getTextContent().empty()) {
        oss << "\n" << indent;
    }
    oss << "</" << element->getTagName() << ">";
    
    return oss.str();
}

std::string CHTLGenerator::generateTextHTML(TextNode* textNode, int indentLevel) {
    if (!textNode) return "";
    
    std::string text = textNode->getText();
    if (text.empty()) return "";
    
    // 转义HTML特殊字符
    text = escapeHTML(text);
    
    return text;
}

std::string CHTLGenerator::generateTemplateHTML(NodePtr node, int indentLevel) {
    // 模板节点在HTML生成时通常被展开
    std::ostringstream oss;
    
    for (const auto& child : node->getChildren()) {
        std::string childHTML = generateHTMLFromNode(child, indentLevel);
        if (!childHTML.empty()) {
            oss << childHTML << "\n";
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateCustomHTML(NodePtr node, int indentLevel) {
    // 自定义节点在HTML生成时通常被展开
    std::ostringstream oss;
    
    for (const auto& child : node->getChildren()) {
        std::string childHTML = generateHTMLFromNode(child, indentLevel);
        if (!childHTML.empty()) {
            oss << childHTML << "\n";
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateStyleHTML(NodePtr node, int indentLevel) {
    std::ostringstream oss;
    std::string indent = getIndent(indentLevel);
    
    oss << indent << "<style";
    
    // 添加属性
    for (const auto& attr : node->getAttributes()) {
        oss << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    oss << ">\n";
    
    // CSS内容
    std::string css = node->getContent();
    if (!css.empty()) {
        oss << css << "\n";
    }
    
    oss << indent << "</style>";
    
    return oss.str();
}

std::string CHTLGenerator::generateScriptHTML(NodePtr node, int indentLevel) {
    std::ostringstream oss;
    std::string indent = getIndent(indentLevel);
    
    oss << indent << "<script";
    
    // 添加属性
    for (const auto& attr : node->getAttributes()) {
        oss << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    oss << ">\n";
    
    // JS内容
    std::string js = node->getContent();
    if (!js.empty()) {
        oss << js << "\n";
    }
    
    oss << indent << "</script>";
    
    return oss.str();
}

std::string CHTLGenerator::generateOriginHTML(NodePtr node, int indentLevel) {
    // 原始HTML直接输出
    return node->getContent();
}

std::string CHTLGenerator::generateImportHTML(NodePtr node, int indentLevel) {
    // 导入节点在HTML生成时通常被处理
    return "";
}

std::string CHTLGenerator::generateConfigurationHTML(NodePtr node, int indentLevel) {
    // 配置节点在HTML生成时通常被忽略
    return "";
}

std::string CHTLGenerator::generateNamespaceHTML(NodePtr node, int indentLevel) {
    // 命名空间节点在HTML生成时通常被展开
    std::ostringstream oss;
    
    for (const auto& child : node->getChildren()) {
        std::string childHTML = generateHTMLFromNode(child, indentLevel);
        if (!childHTML.empty()) {
            oss << childHTML << "\n";
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateOperatorHTML(NodePtr node, int indentLevel) {
    // 操作节点在HTML生成时通常被忽略
    return "";
}

std::string CHTLGenerator::generateCSSFromNode(NodePtr node) {
    if (!node) return "";
    
    std::ostringstream oss;
    
    // 生成元素的CSS
    if (node->getType() == NodeType::ELEMENT) {
        auto element = std::dynamic_pointer_cast<ElementNode>(node);
        if (element) {
            oss << generateElementCSS(element.get());
        }
    }
    
    // 生成子节点的CSS
    for (const auto& child : node->getChildren()) {
        std::string childCSS = generateCSSFromNode(child);
        if (!childCSS.empty()) {
            oss << childCSS << "\n";
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateElementCSS(ElementNode* element) {
    if (!element) return "";
    
    std::ostringstream oss;
    
    // 生成基于类名的CSS规则
    for (const auto& className : element->getClasses()) {
        oss << "." << className << " {\n";
        
        // 内联样式转换为CSS规则
        for (const auto& style : element->getInlineStyles()) {
            oss << "    " << style.first << ": " << style.second << ";\n";
        }
        
        oss << "}\n\n";
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateJSFromNode(NodePtr node) {
    if (!node) return "";
    
    std::ostringstream oss;
    
    // 生成元素的JS
    if (node->getType() == NodeType::ELEMENT) {
        auto element = std::dynamic_pointer_cast<ElementNode>(node);
        if (element) {
            oss << generateElementJS(element.get());
        }
    }
    
    // 生成子节点的JS
    for (const auto& child : node->getChildren()) {
        std::string childJS = generateJSFromNode(child);
        if (!childJS.empty()) {
            oss << childJS << "\n";
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateElementJS(ElementNode* element) {
    if (!element) return "";
    
    std::ostringstream oss;
    
    // 生成基于类名的JS选择器
    if (!element->getClasses().empty()) {
        oss << "// Element: " << element->getTagName() << " with classes: ";
        bool first = true;
        for (const auto& className : element->getClasses()) {
            if (!first) oss << ", ";
            oss << className;
            first = false;
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string CHTLGenerator::getIndent(int level) const {
    return std::string(level * 2, ' ');
}

std::string CHTLGenerator::formatHTML(const std::string& html) const {
    // 简单的HTML格式化
    std::string result = html;
    
    // 在标签后添加换行
    result = std::regex_replace(result, std::regex(">"), ">\n");
    result = std::regex_replace(result, std::regex("<"), "\n<");
    
    return result;
}

std::string CHTLGenerator::formatCSS(const std::string& css) const {
    // 简单的CSS格式化
    std::string result = css;
    
    // 在规则后添加换行
    result = std::regex_replace(result, std::regex("}"), "}\n");
    result = std::regex_replace(result, std::regex("\\{"), "{\n");
    
    return result;
}

std::string CHTLGenerator::formatJS(const std::string& js) const {
    // 简单的JS格式化
    std::string result = js;
    
    // 在语句后添加换行
    result = std::regex_replace(result, std::regex(";"), ";\n");
    
    return result;
}

std::string CHTLGenerator::minifyHTML(const std::string& html) const {
    std::string result = html;
    
    // 移除多余的空白字符
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex(">\\s+<"), "><");
    
    return result;
}

std::string CHTLGenerator::minifyCSS(const std::string& css) const {
    std::string result = css;
    
    // 移除多余的空白字符
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*\\{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*\\}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    
    return result;
}

std::string CHTLGenerator::minifyJS(const std::string& js) const {
    std::string result = js;
    
    // 移除多余的空白字符
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*\\{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*\\}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    
    return result;
}

std::string CHTLGenerator::generateDefaultHTMLStructure() const {
    return R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL Generated Page</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
)";
}

std::string CHTLGenerator::generateDefaultCSSStructure() const {
    return R"(/* CHTL Generated CSS */
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: Arial, sans-serif;
    line-height: 1.6;
    color: #333;
}

)";
}

std::string CHTLGenerator::generateDefaultJSStructure() const {
    return R"(// CHTL Generated JavaScript
document.addEventListener('DOMContentLoaded', function() {
    // Your code here
});

)";
}

void CHTLGenerator::processTemplates(NodePtr root) {
    // 处理模板节点
    for (const auto& child : root->getChildren()) {
        if (child->getType() == NodeType::TEMPLATE) {
            processTemplate(child);
        }
    }
}

void CHTLGenerator::processTemplate(NodePtr templateNode) {
    // 模板处理逻辑
    // 这里可以实现模板展开、继承等功能
}

void CHTLGenerator::processCustom(NodePtr customNode) {
    // 自定义节点处理逻辑
    // 这里可以实现自定义节点的特例化等功能
}

void CHTLGenerator::processImports(NodePtr root) {
    // 处理导入节点
    for (const auto& child : root->getChildren()) {
        if (child->getType() == NodeType::IMPORT) {
            processImport(child);
        }
    }
}

void CHTLGenerator::processImport(NodePtr importNode) {
    // 导入处理逻辑
    // 这里可以实现模块导入、文件解析等功能
}

void CHTLGenerator::processConfiguration(NodePtr root) {
    // 处理配置节点
    for (const auto& child : root->getChildren()) {
        if (child->getType() == NodeType::CONFIG) {
            processConfigurationNode(child);
        }
    }
}

void CHTLGenerator::processConfigurationNode(NodePtr configNode) {
    // 配置处理逻辑
    // 这里可以实现配置解析、应用等功能
}

void CHTLGenerator::processNamespaces(NodePtr root) {
    // 处理命名空间节点
    for (const auto& child : root->getChildren()) {
        if (child->getType() == NodeType::NAMESPACE) {
            processNamespace(child);
        }
    }
}

void CHTLGenerator::processNamespace(NodePtr namespaceNode) {
    // 命名空间处理逻辑
    // 这里可以实现命名空间解析、作用域管理等功能
}

void CHTLGenerator::setErrorHandler(std::function<void(const std::string&)> handler) {
    errorHandler_ = handler;
}

void CHTLGenerator::setWarningHandler(std::function<void(const std::string&)> handler) {
    warningHandler_ = handler;
}

void CHTLGenerator::addError(const std::string& error) {
    errors_.push_back(error);
}

void CHTLGenerator::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

void CHTLGenerator::reportError(const std::string& message) {
    if (errorHandler_) {
        errorHandler_(message);
    } else {
        addError(message);
    }
}

void CHTLGenerator::reportWarning(const std::string& message) {
    if (warningHandler_) {
        warningHandler_(message);
    } else {
        addWarning(message);
    }
}

bool CHTLGenerator::validateNode(NodePtr node) {
    if (!node) return false;
    
    // 基本验证
    if (node->getType() == NodeType::ELEMENT) {
        auto element = std::dynamic_pointer_cast<ElementNode>(node);
        return validateElement(element.get());
    }
    
    return true;
}

bool CHTLGenerator::validateElement(ElementNode* element) {
    if (!element) return false;
    
    // 验证元素
    if (element->getTagName().empty()) {
        addError("Element tag name cannot be empty");
        return false;
    }
    
    return true;
}

bool CHTLGenerator::validateTemplate(NodePtr templateNode) {
    if (!templateNode) return false;
    
    // 验证模板
    if (templateNode->getName().empty()) {
        addError("Template name cannot be empty");
        return false;
    }
    
    return true;
}

bool CHTLGenerator::validateCustom(NodePtr customNode) {
    if (!customNode) return false;
    
    // 验证自定义节点
    if (customNode->getName().empty()) {
        addError("Custom node name cannot be empty");
        return false;
    }
    
    return true;
}

} // namespace CHTL