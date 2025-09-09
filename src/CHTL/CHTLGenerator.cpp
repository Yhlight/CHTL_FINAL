#include "CHTL/CHTLGenerator.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLGenerator::CHTLGenerator(std::shared_ptr<CHTLContext> context) 
    : m_context(context), m_prettyPrint(true), m_minify(false), m_includeComments(true),
      m_generatedLines(0), m_generatedSize(0) {
}

CHTLGenerator::~CHTLGenerator() = default;

std::string CHTLGenerator::generateHTML(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return "";
    }
    
    std::stringstream ss;
    m_generatedLines = 0;
    m_generatedSize = 0;
    
    // 生成HTML5文档声明
    if (m_includeComments) {
        ss << "<!DOCTYPE html>" << std::endl;
        m_generatedLines++;
    }
    
    // 生成HTML内容
    std::string html = generateElement(root, 0);
    ss << html;
    
    m_generatedSize = ss.str().length();
    return ss.str();
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return "";
    }
    
    std::stringstream ss;
    
    // 遍历AST查找样式节点
    generateCSSFromNode(root, ss, 0);
    
    return ss.str();
}

std::string CHTLGenerator::generateJavaScript(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return "";
    }
    
    std::stringstream ss;
    
    // 遍历AST查找脚本节点
    generateJavaScriptFromNode(root, ss, 0);
    
    return ss.str();
}

std::string CHTLGenerator::generateDocument(std::shared_ptr<BaseNode> root) {
    std::string html = generateHTML(root);
    std::string css = generateCSS(root);
    std::string js = generateJavaScript(root);
    
    return mergeHTMLAndCSS(mergeHTMLAndJavaScript(html, js), css);
}

void CHTLGenerator::setPrettyPrint(bool enabled) {
    m_prettyPrint = enabled;
}

void CHTLGenerator::setMinify(bool enabled) {
    m_minify = enabled;
}

void CHTLGenerator::setIncludeComments(bool enabled) {
    m_includeComments = enabled;
}

size_t CHTLGenerator::getGeneratedLines() const {
    return m_generatedLines;
}

size_t CHTLGenerator::getGeneratedSize() const {
    return m_generatedSize;
}

std::string CHTLGenerator::generateElement(std::shared_ptr<BaseNode> node, int indent) {
    if (!node) {
        return "";
    }
    
    // 动态转换为ElementNode
    auto elementNode = std::dynamic_pointer_cast<ElementNode>(node);
    if (!elementNode) {
        return "";
    }
    
    std::stringstream ss;
    std::string indentStr = this->indent(indent);
    
    // 开始标签
    ss << indentStr << "<" << elementNode->getTagName();
    
    // 添加属性
    std::string attributes = generateAttributes(node);
    if (!attributes.empty()) {
        ss << " " << attributes;
    }
    
    // 添加样式属性
    std::string styleAttrs = generateStyleAttributes(node);
    if (!styleAttrs.empty()) {
        ss << " " << styleAttrs;
    }
    
    // 自闭合标签
    if (elementNode->isSelfClosing()) {
        ss << " />";
        if (m_prettyPrint) {
            ss << std::endl;
        }
        return ss.str();
    }
    
    ss << ">";
    if (m_prettyPrint) {
        ss << std::endl;
    }
    
    // 添加文本内容
    if (!elementNode->getTextContent().empty()) {
        ss << this->indent(indent + 1) << escapeHTML(elementNode->getTextContent());
        if (m_prettyPrint) {
            ss << std::endl;
        }
    }
    
    // 添加子节点
    for (const auto& child : elementNode->getChildren()) {
        ss << generateElement(child, indent + 1);
    }
    
    // 结束标签
    ss << indentStr << "</" << elementNode->getTagName() << ">";
    if (m_prettyPrint) {
        ss << std::endl;
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateText(std::shared_ptr<BaseNode> node) {
    auto textNode = std::dynamic_pointer_cast<TextNode>(node);
    if (!textNode) {
        return "";
    }
    
    return escapeHTML(textNode->getText());
}

std::string CHTLGenerator::generateComment(std::shared_ptr<BaseNode> node) {
    auto commentNode = std::dynamic_pointer_cast<CommentNode>(node);
    if (!commentNode) {
        return "";
    }
    
    if (!m_includeComments) {
        return "";
    }
    
    std::stringstream ss;
    std::string indentStr = this->indent(0);
    
    switch (commentNode->getCommentType()) {
        case CommentType::LINE:
            ss << indentStr << "<!-- " << commentNode->getCommentContent() << " -->";
            break;
        case CommentType::BLOCK:
            ss << indentStr << "<!-- " << commentNode->getCommentContent() << " -->";
            break;
        case CommentType::GENERATOR:
            ss << indentStr << "<!-- " << commentNode->getCommentContent() << " -->";
            break;
    }
    
    if (m_prettyPrint) {
        ss << std::endl;
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateAttributes(std::shared_ptr<BaseNode> node) {
    std::stringstream ss;
    bool first = true;
    
    for (const auto& attr : node->getAttributes()) {
        if (!first) {
            ss << " ";
        }
        ss << formatAttribute(attr.first, attr.second);
        first = false;
    }
    
    return ss.str();
}

std::string CHTLGenerator::generateStyleAttributes(std::shared_ptr<BaseNode> node) {
    auto elementNode = std::dynamic_pointer_cast<ElementNode>(node);
    if (!elementNode) {
        return "";
    }
    
    const auto& styleProps = elementNode->getStyleProperties();
    if (styleProps.empty()) {
        return "";
    }
    
    std::stringstream ss;
    ss << "style=\"";
    bool first = true;
    
    for (const auto& prop : styleProps) {
        if (!first) {
            ss << "; ";
        }
        ss << formatStyleProperty(prop.first, prop.second);
        first = false;
    }
    
    ss << "\"";
    return ss.str();
}

std::string CHTLGenerator::escapeHTML(const std::string& text) {
    std::string result = text;
    
    // 替换HTML特殊字符
    size_t pos = 0;
    while ((pos = result.find('&', pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    pos = 0;
    while ((pos = result.find('<', pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = result.find('>', pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = result.find('"', pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    pos = 0;
    while ((pos = result.find('\'', pos)) != std::string::npos) {
        result.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return result;
}

std::string CHTLGenerator::escapeCSS(const std::string& text) {
    // TODO: 实现CSS转义
    return text;
}

std::string CHTLGenerator::escapeJavaScript(const std::string& text) {
    // TODO: 实现JavaScript转义
    return text;
}

std::string CHTLGenerator::indent(int level) {
    if (!m_prettyPrint) {
        return "";
    }
    
    return std::string(level * 2, ' ');
}

std::string CHTLGenerator::formatAttribute(const std::string& key, const std::string& value) {
    return key + "=\"" + escapeHTML(value) + "\"";
}

std::string CHTLGenerator::formatStyleProperty(const std::string& property, const std::string& value) {
    return property + ": " + value;
}

std::string CHTLGenerator::mergeHTMLAndCSS(const std::string& html, const std::string& css) {
    if (css.empty()) {
        return html;
    }
    
    // 在</head>标签前插入CSS
    size_t headEnd = html.find("</head>");
    if (headEnd != std::string::npos) {
        return html.substr(0, headEnd) + 
               "\n<style>\n" + css + "\n</style>\n" + 
               html.substr(headEnd);
    }
    
    return html;
}

std::string CHTLGenerator::mergeHTMLAndJavaScript(const std::string& html, const std::string& js) {
    if (js.empty()) {
        return html;
    }
    
    // 在</body>标签前插入JavaScript
    size_t bodyEnd = html.find("</body>");
    if (bodyEnd != std::string::npos) {
        return html.substr(0, bodyEnd) + 
               "\n<script>\n" + js + "\n</script>\n" + 
               html.substr(bodyEnd);
    }
    
    return html;
}

void CHTLGenerator::generateCSSFromNode(std::shared_ptr<BaseNode> node, std::stringstream& ss, int indent) {
    if (!node) {
        return;
    }
    
    // 检查是否是样式节点
    if (node->getType() == NodeType::STYLE) {
        // 生成样式块
        std::string styleBlock = generateStyleBlock(node);
        if (!styleBlock.empty()) {
            ss << styleBlock;
            if (m_prettyPrint) {
                ss << std::endl;
            }
        }
    }
    
    // 递归处理子节点
    for (const auto& child : node->getChildren()) {
        generateCSSFromNode(child, ss, indent);
    }
}

void CHTLGenerator::generateJavaScriptFromNode(std::shared_ptr<BaseNode> node, std::stringstream& ss, int indent) {
    if (!node) {
        return;
    }
    
    // 检查是否是脚本节点
    if (node->getType() == NodeType::SCRIPT) {
        // 生成脚本块
        std::string scriptBlock = generateScriptBlock(node);
        if (!scriptBlock.empty()) {
            ss << scriptBlock;
            if (m_prettyPrint) {
                ss << std::endl;
            }
        }
    }
    
    // 递归处理子节点
    for (const auto& child : node->getChildren()) {
        generateJavaScriptFromNode(child, ss, indent);
    }
}

std::string CHTLGenerator::generateStyleBlock(std::shared_ptr<BaseNode> node) {
    // TODO: 实现样式块生成
    return "";
}

std::string CHTLGenerator::generateScriptBlock(std::shared_ptr<BaseNode> node) {
    // TODO: 实现脚本块生成
    return "";
}

} // namespace CHTL