#include "CHTL/CHTLGenerator.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/TemplateNode.h"
#include "CHTL/CHTLNode/CustomNode.h"
#include "CHTL/CHTLNode/ImportNode.h"
#include "CHTL/CHTLNode/NamespaceNode.h"
#include "CHTL/CHTLNode/ConfigurationNode.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLGenerator::CHTLGenerator(std::shared_ptr<CHTLContext> context)
    : m_context(context)
    , m_generateDefaultStructure(true)
    , m_generateCSS(true)
    , m_generateJavaScript(true)
    , m_debugMode(false)
{
}

CHTLGenerator::~CHTLGenerator() {
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<BaseNode> ast) {
    if (!ast) return "";
    
    m_htmlCode.clear();
    
    if (m_debugMode) {
        std::cout << "[CHTLGenerator] Generating HTML from AST" << std::endl;
    }
    
    // 遍历AST生成HTML
    for (size_t i = 0; i < ast->getChildCount(); ++i) {
        auto child = ast->getChild(i);
        if (child) {
            m_htmlCode += generateNode(child, 0);
        }
    }
    
    return m_htmlCode;
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<BaseNode> ast) {
    if (!ast) return "";
    
    m_cssCode.clear();
    
    if (m_debugMode) {
        std::cout << "[CHTLGenerator] Generating CSS from AST" << std::endl;
    }
    
    // 收集所有样式节点
    std::vector<std::shared_ptr<BaseNode>> styles;
    collectStyles(ast, styles);
    
    // 生成CSS代码
    for (const auto& style : styles) {
        m_cssCode += generateStyle(style, 0);
        m_cssCode += "\n";
    }
    
    return m_cssCode;
}

std::string CHTLGenerator::generateJavaScript(std::shared_ptr<BaseNode> ast) {
    if (!ast) return "";
    
    m_javascriptCode.clear();
    
    if (m_debugMode) {
        std::cout << "[CHTLGenerator] Generating JavaScript from AST" << std::endl;
    }
    
    // 收集所有脚本节点
    std::vector<std::shared_ptr<BaseNode>> scripts;
    collectScripts(ast, scripts);
    
    // 生成JavaScript代码
    for (const auto& script : scripts) {
        m_javascriptCode += generateScript(script, 0);
        m_javascriptCode += "\n";
    }
    
    return m_javascriptCode;
}

std::string CHTLGenerator::generateDocument(std::shared_ptr<BaseNode> ast) {
    std::ostringstream oss;
    
    // HTML5文档声明
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    oss << "  <meta charset=\"UTF-8\">\n";
    oss << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    oss << "  <title>CHTL Generated Page</title>\n";
    
    // 添加CSS
    if (m_generateCSS) {
        std::string css = generateCSS(ast);
        if (!css.empty()) {
            oss << "  <style>\n";
            oss << css;
            oss << "  </style>\n";
        }
    }
    
    oss << "</head>\n";
    oss << "<body>\n";
    
    // 添加HTML内容
    std::string html = generateHTML(ast);
    if (!html.empty()) {
        oss << html;
    }
    
    // 添加JavaScript
    if (m_generateJavaScript) {
        std::string js = generateJavaScript(ast);
        if (!js.empty()) {
            oss << "  <script>\n";
            oss << js;
            oss << "  </script>\n";
        }
    }
    
    oss << "</body>\n";
    oss << "</html>\n";
    
    return oss.str();
}

void CHTLGenerator::setGenerateDefaultStructure(bool generateDefault) {
    m_generateDefaultStructure = generateDefault;
}

void CHTLGenerator::setGenerateCSS(bool generateCSS) {
    m_generateCSS = generateCSS;
}

void CHTLGenerator::setGenerateJavaScript(bool generateJS) {
    m_generateJavaScript = generateJS;
}

void CHTLGenerator::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool CHTLGenerator::isDebugMode() const {
    return m_debugMode;
}

std::string CHTLGenerator::generateNode(std::shared_ptr<BaseNode> node, int indent) {
    if (!node) return "";
    
    switch (node->getType()) {
        case NodeType::ELEMENT:
            return generateElement(node, indent);
        case NodeType::TEXT:
            return generateText(node, indent);
        case NodeType::COMMENT:
            return generateComment(node, indent);
        case NodeType::STYLE:
            return generateStyle(node, indent);
        case NodeType::SCRIPT:
            return generateScript(node, indent);
        case NodeType::TEMPLATE:
            return generateTemplate(node, indent);
        case NodeType::CUSTOM:
            return generateCustom(node, indent);
        case NodeType::IMPORT:
            return generateImport(node, indent);
        case NodeType::NAMESPACE:
            return generateNamespace(node, indent);
        case NodeType::CONFIG:
            return generateConfiguration(node, indent);
        default:
            return "";
    }
}

std::string CHTLGenerator::generateElement(std::shared_ptr<BaseNode> node, int indent) {
    if (!node) return "";
    
    std::ostringstream oss;
    std::string indentStr = generateIndent(indent);
    
    // 获取标签名
    std::string tagName = node->getName();
    if (tagName.empty()) {
        tagName = "div"; // 默认标签
    }
    
    // 生成开始标签
    oss << indentStr << "<" << tagName;
    
    // 生成属性
    std::string attributes = generateAttributes(node);
    if (!attributes.empty()) {
        oss << " " << attributes;
    }
    
    // 检查是否为自闭合标签
    auto elementNode = std::dynamic_pointer_cast<ElementNode>(node);
    if (elementNode && elementNode->isSelfClosing()) {
        oss << " />\n";
        return oss.str();
    }
    
    oss << ">\n";
    
    // 生成子节点
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child = node->getChild(i);
        if (child) {
            oss << generateNode(child, indent + 1);
        }
    }
    
    // 生成结束标签
    oss << indentStr << "</" << tagName << ">\n";
    
    return oss.str();
}

std::string CHTLGenerator::generateText(std::shared_ptr<BaseNode> node, int indent) {
    if (!node) return "";
    
    std::ostringstream oss;
    std::string indentStr = generateIndent(indent);
    
    std::string text = node->getValue();
    if (!text.empty()) {
        // 检查是否为生成器注释
        auto textNode = std::dynamic_pointer_cast<TextNode>(node);
        if (textNode && textNode->isGeneratorComment()) {
            oss << indentStr << "<!-- " << text << " -->\n";
        } else {
            // 转义HTML特殊字符
            text = escapeHtml(text);
            oss << indentStr << text << "\n";
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateComment(std::shared_ptr<BaseNode> node, int indent) {
    if (!node) return "";
    
    std::ostringstream oss;
    std::string indentStr = generateIndent(indent);
    
    std::string comment = node->getValue();
    if (!comment.empty()) {
        oss << indentStr << "<!-- " << comment << " -->\n";
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateStyle(std::shared_ptr<BaseNode> node, int indent) {
    if (!node) return "";
    
    std::ostringstream oss;
    std::string indentStr = generateIndent(indent);
    
    // 生成CSS规则
    const auto& attributes = node->getAttributes();
    for (const auto& attr : attributes) {
        oss << indentStr << attr.first << ": " << attr.second << ";\n";
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateScript(std::shared_ptr<BaseNode> node, int indent) {
    if (!node) return "";
    
    std::ostringstream oss;
    std::string indentStr = generateIndent(indent);
    
    std::string script = node->getValue();
    if (!script.empty()) {
        oss << indentStr << script << "\n";
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateTemplate(std::shared_ptr<BaseNode> node, int indent) {
    if (!node) return "";
    
    auto templateNode = std::dynamic_pointer_cast<TemplateNode>(node);
    if (!templateNode) return "";
    
    std::ostringstream oss;
    std::string indentStr = generateIndent(indent);
    
    // 生成模板注释
    oss << indentStr << "<!-- Template: " << templateNode->getTemplateName() << " -->\n";
    
    // 生成模板内容
    for (size_t i = 0; i < templateNode->getChildCount(); ++i) {
        auto child = templateNode->getChild(i);
        if (child) {
            oss << generateNode(child, indent);
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateCustom(std::shared_ptr<BaseNode> node, int indent) {
    if (!node) return "";
    
    auto customNode = std::dynamic_pointer_cast<CustomNode>(node);
    if (!customNode) return "";
    
    std::ostringstream oss;
    std::string indentStr = generateIndent(indent);
    
    // 生成自定义注释
    oss << indentStr << "<!-- Custom: " << customNode->getCustomName() << " -->\n";
    
    // 生成自定义内容
    for (size_t i = 0; i < customNode->getChildCount(); ++i) {
        auto child = customNode->getChild(i);
        if (child) {
            oss << generateNode(child, indent);
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateImport(std::shared_ptr<BaseNode> node, int indent) {
    if (!node) return "";
    
    auto importNode = std::dynamic_pointer_cast<ImportNode>(node);
    if (!importNode) return "";
    
    std::ostringstream oss;
    std::string indentStr = generateIndent(indent);
    
    // 生成导入注释
    oss << indentStr << "<!-- Import: " << importNode->getFilePath() << " -->\n";
    
    // 根据导入类型生成内容
    std::string content = importNode->getImportContent();
    if (!content.empty()) {
        switch (importNode->getImportType()) {
            case ImportType::HTML:
                // HTML内容直接嵌入
                oss << content << "\n";
                break;
            case ImportType::CSS:
                // CSS内容包装在style标签中
                oss << indentStr << "<style>\n";
                oss << content << "\n";
                oss << indentStr << "</style>\n";
                break;
            case ImportType::JS:
                // JavaScript内容包装在script标签中
                oss << indentStr << "<script>\n";
                oss << content << "\n";
                oss << indentStr << "</script>\n";
                break;
            case ImportType::CHTL:
                // CHTL内容需要进一步处理
                oss << indentStr << "<!-- CHTL content: " << content.substr(0, 100) << "... -->\n";
                break;
            default:
                // 其他类型的内容
                oss << indentStr << "<!-- " << content.substr(0, 100) << "... -->\n";
                break;
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateNamespace(std::shared_ptr<BaseNode> node, int indent) {
    if (!node) return "";
    
    auto namespaceNode = std::dynamic_pointer_cast<NamespaceNode>(node);
    if (!namespaceNode) return "";
    
    std::ostringstream oss;
    std::string indentStr = generateIndent(indent);
    
    // 生成命名空间注释
    oss << indentStr << "<!-- Namespace: " << namespaceNode->getNamespaceName() << " -->\n";
    
    // 生成命名空间内容
    for (size_t i = 0; i < namespaceNode->getChildCount(); ++i) {
        auto child = namespaceNode->getChild(i);
        if (child) {
            oss << generateNode(child, indent);
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateConfiguration(std::shared_ptr<BaseNode> node, int indent) {
    if (!node) return "";
    
    auto configurationNode = std::dynamic_pointer_cast<ConfigurationNode>(node);
    if (!configurationNode) return "";
    
    std::ostringstream oss;
    std::string indentStr = generateIndent(indent);
    
    // 生成配置注释
    oss << indentStr << "<!-- Configuration: " << configurationNode->getConfigurationName() << " -->\n";
    
    // 生成配置内容
    const auto& items = configurationNode->getConfigurationItems();
    if (!items.empty()) {
        oss << indentStr << "<!-- Configuration Items -->\n";
        for (const auto& item : items) {
            oss << indentStr << "<!-- " << item.first << ": " << item.second << " -->\n";
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateAttributes(std::shared_ptr<BaseNode> node) {
    if (!node) return "";
    
    std::ostringstream oss;
    const auto& attributes = node->getAttributes();
    
    bool first = true;
    for (const auto& attr : attributes) {
        if (!first) {
            oss << " ";
        }
        oss << attr.first << "=\"" << escapeHtml(attr.second) << "\"";
        first = false;
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateIndent(int level) const {
    return std::string(level * 2, ' ');
}

std::string CHTLGenerator::escapeHtml(const std::string& text) const {
    std::string result;
    result.reserve(text.length() * 2);
    
    for (char c : text) {
        switch (c) {
            case '&':  result += "&amp;";  break;
            case '<':  result += "&lt;";   break;
            case '>':  result += "&gt;";   break;
            case '"':  result += "&quot;"; break;
            case '\'': result += "&#39;";  break;
            default:   result += c;        break;
        }
    }
    
    return result;
}

std::string CHTLGenerator::escapeCSS(const std::string& text) const {
    // CSS转义逻辑
    return text;
}

std::string CHTLGenerator::escapeJavaScript(const std::string& text) const {
    // JavaScript转义逻辑
    return text;
}

void CHTLGenerator::collectStyles(std::shared_ptr<BaseNode> node, std::vector<std::shared_ptr<BaseNode>>& styles) {
    if (!node) return;
    
    if (node->getType() == NodeType::STYLE) {
        styles.push_back(node);
    }
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child = node->getChild(i);
        if (child) {
            collectStyles(child, styles);
        }
    }
}

void CHTLGenerator::collectScripts(std::shared_ptr<BaseNode> node, std::vector<std::shared_ptr<BaseNode>>& scripts) {
    if (!node) return;
    
    if (node->getType() == NodeType::SCRIPT) {
        scripts.push_back(node);
    }
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        auto child = node->getChild(i);
        if (child) {
            collectScripts(child, scripts);
        }
    }
}

void CHTLGenerator::addError(const std::string& message) {
    if (m_context) {
        m_context->addError("Generator Error: " + message);
    }
}

void CHTLGenerator::addWarning(const std::string& message) {
    if (m_context) {
        m_context->addWarning("Generator Warning: " + message);
    }
}

} // namespace CHTL