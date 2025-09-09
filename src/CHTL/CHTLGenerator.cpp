#include "CHTL/CHTLGenerator.h"
#include <sstream>

namespace CHTL {

// HTMLGenerator 实现
HTMLGenerator::HTMLGenerator(std::shared_ptr<CHTLContext> context)
    : m_context(context), m_indentLevel(0) {
}

std::string HTMLGenerator::generate(std::shared_ptr<BaseNode> root) {
    m_output.clear();
    m_indentLevel = 0;
    
    if (root) {
        root->accept(*this);
    }
    
    return m_output;
}

void HTMLGenerator::visit(ElementNode& node) {
    if (node.getTagName() == "program") {
        // 程序根节点，遍历子节点
        for (const auto& child : node.getChildren()) {
            child->accept(*this);
        }
        return;
    }
    
    // 生成开始标签
    appendIndented("<" + node.getTagName());
    
    // 生成属性
    std::string attributes = generateAttributes(node.getAttributes());
    if (!attributes.empty()) {
        append(" " + attributes);
    }
    
    // 检查是否为自闭合标签
    std::vector<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    bool isSelfClosing = std::find(selfClosingTags.begin(), selfClosingTags.end(), 
                                  node.getTagName()) != selfClosingTags.end();
    
    if (isSelfClosing && node.getChildren().empty()) {
        append(" />");
        newline();
        return;
    }
    
    append(">");
    newline();
    
    // 生成子节点
    if (!node.getChildren().empty()) {
        m_indentLevel++;
        for (const auto& child : node.getChildren()) {
            child->accept(*this);
        }
        m_indentLevel--;
        appendIndented("</" + node.getTagName() + ">");
        newline();
    } else {
        // 没有子节点，闭合标签
        append("</" + node.getTagName() + ">");
        newline();
    }
}

void HTMLGenerator::visit(TextNode& node) {
    appendIndented(node.getContent());
    newline();
}

void HTMLGenerator::visit(CommentNode& node) {
    if (!node.isGenerator()) {
        // 只生成非生成器注释
        appendIndented("<!-- " + node.getContent() + " -->");
        newline();
    }
}

void HTMLGenerator::visit(TemplateNode& node) {
    // 模板节点不直接生成 HTML，但需要处理其内容
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void HTMLGenerator::visit(CustomNode& node) {
    // 自定义节点不直接生成 HTML，但需要处理其内容
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void HTMLGenerator::visit(StyleNode& node) {
    // 样式节点生成 <style> 标签
    appendIndented("<style>");
    newline();
    
    m_indentLevel++;
    for (const auto& [key, value] : node.getAttributes()) {
        appendIndented(key + ": " + value + ";");
        newline();
    }
    m_indentLevel--;
    
    appendIndented("</style>");
    newline();
}

void HTMLGenerator::visit(ScriptNode& node) {
    // 脚本节点生成 <script> 标签
    appendIndented("<script>");
    newline();
    
    if (node.hasAttribute("content")) {
        m_indentLevel++;
        appendIndented(node.getAttribute("content"));
        newline();
        m_indentLevel--;
    }
    
    appendIndented("</script>");
    newline();
}

void HTMLGenerator::visit(OriginNode& node) {
    // 原始嵌入节点直接输出内容
    if (node.hasAttribute("content")) {
        appendIndented(node.getAttribute("content"));
        newline();
    }
}

void HTMLGenerator::visit(ImportNode& node) {
    // 导入节点不直接生成 HTML
    // 导入的内容会在处理时被包含到相应位置
}

void HTMLGenerator::visit(ConfigNode& node) {
    // 配置节点不生成 HTML
}

void HTMLGenerator::visit(NamespaceNode& node) {
    // 命名空间节点不直接生成 HTML，但需要处理其内容
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void HTMLGenerator::visit(OperatorNode& node) {
    // 操作符节点不生成 HTML
}

void HTMLGenerator::indent() {
    for (int i = 0; i < m_indentLevel; ++i) {
        m_output += "  ";
    }
}

void HTMLGenerator::newline() {
    m_output += "\n";
}

void HTMLGenerator::append(const std::string& text) {
    m_output += text;
}

void HTMLGenerator::appendIndented(const std::string& text) {
    indent();
    append(text);
}

std::string HTMLGenerator::generateElement(ElementNode& node) {
    std::stringstream ss;
    ss << "<" << node.getTagName();
    
    std::string attributes = generateAttributes(node.getAttributes());
    if (!attributes.empty()) {
        ss << " " << attributes;
    }
    
    ss << ">";
    return ss.str();
}

std::string HTMLGenerator::generateAttributes(const std::unordered_map<std::string, std::string>& attributes) {
    std::stringstream ss;
    bool first = true;
    
    for (const auto& [key, value] : attributes) {
        if (!first) {
            ss << " ";
        }
        ss << key << "=\"" << value << "\"";
        first = false;
    }
    
    return ss.str();
}

std::string HTMLGenerator::generateText(TextNode& node) {
    return node.getContent();
}

std::string HTMLGenerator::generateComment(CommentNode& node) {
    if (node.isGenerator()) {
        return "";
    }
    return "<!-- " + node.getContent() + " -->";
}

std::string HTMLGenerator::generateStyle(StyleNode& node) {
    std::stringstream ss;
    ss << "<style>\n";
    
    for (const auto& [key, value] : node.getAttributes()) {
        ss << "  " << key << ": " << value << ";\n";
    }
    
    ss << "</style>";
    return ss.str();
}

std::string HTMLGenerator::generateGlobalStyle() {
    // 生成全局样式
    return "";
}

std::string HTMLGenerator::generateScript(ScriptNode& node) {
    std::stringstream ss;
    ss << "<script>\n";
    
    if (node.hasAttribute("content")) {
        ss << node.getAttribute("content") << "\n";
    }
    
    ss << "</script>";
    return ss.str();
}

std::string HTMLGenerator::generateGlobalScript() {
    // 生成全局脚本
    return "";
}

void HTMLGenerator::processTemplates() {
    // 处理模板
}

void HTMLGenerator::processCustoms() {
    // 处理自定义
}

void HTMLGenerator::processImports() {
    // 处理导入
}

void HTMLGenerator::processNamespaces() {
    // 处理命名空间
}

void HTMLGenerator::processConfiguration() {
    // 处理配置
}

// CSSGenerator 实现
CSSGenerator::CSSGenerator(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string CSSGenerator::generate(std::shared_ptr<BaseNode> root) {
    m_output.clear();
    
    if (root) {
        root->accept(*this);
    }
    
    return m_output;
}

void CSSGenerator::visit(ElementNode& node) {
    // 元素节点不直接生成 CSS
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void CSSGenerator::visit(TextNode& node) {
    // 文本节点不生成 CSS
}

void CSSGenerator::visit(CommentNode& node) {
    // 注释节点不生成 CSS
}

void CSSGenerator::visit(TemplateNode& node) {
    // 模板节点生成 CSS
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void CSSGenerator::visit(CustomNode& node) {
    // 自定义节点生成 CSS
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void CSSGenerator::visit(StyleNode& node) {
    // 样式节点生成 CSS 规则
    std::string selector = node.getAttribute("selector");
    if (selector.empty()) {
        selector = "body"; // 默认选择器
    }
    
    std::string rule = generateCSSRule(selector, node.getAttributes());
    if (!rule.empty()) {
        m_output += rule + "\n";
    }
}

void CSSGenerator::visit(ScriptNode& node) {
    // 脚本节点不生成 CSS
}

void CSSGenerator::visit(OriginNode& node) {
    // 原始嵌入节点直接输出 CSS 内容
    if (node.getOriginType() == "@Style" && node.hasAttribute("content")) {
        m_output += node.getAttribute("content") + "\n";
    }
}

void CSSGenerator::visit(ImportNode& node) {
    // 导入节点不直接生成 CSS
}

void CSSGenerator::visit(ConfigNode& node) {
    // 配置节点不生成 CSS
}

void CSSGenerator::visit(NamespaceNode& node) {
    // 命名空间节点不直接生成 CSS
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void CSSGenerator::visit(OperatorNode& node) {
    // 操作符节点不生成 CSS
}

void CSSGenerator::processStyleBlocks() {
    // 处理样式块
}

void CSSGenerator::processTemplates() {
    // 处理模板
}

void CSSGenerator::processCustoms() {
    // 处理自定义
}

std::string CSSGenerator::generateCSSRule(const std::string& selector, 
                                         const std::unordered_map<std::string, std::string>& properties) {
    std::stringstream ss;
    ss << selector << " {\n";
    
    for (const auto& [key, value] : properties) {
        if (key != "selector") { // 跳过选择器属性
            ss << "  " << generateCSSProperty(key, value) << "\n";
        }
    }
    
    ss << "}";
    return ss.str();
}

std::string CSSGenerator::generateCSSProperty(const std::string& name, const std::string& value) {
    return name + ": " + value + ";";
}

// JavaScriptGenerator 实现
JavaScriptGenerator::JavaScriptGenerator(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string JavaScriptGenerator::generate(std::shared_ptr<BaseNode> root) {
    m_output.clear();
    
    if (root) {
        root->accept(*this);
    }
    
    return m_output;
}

void JavaScriptGenerator::visit(ElementNode& node) {
    // 元素节点不直接生成 JavaScript
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void JavaScriptGenerator::visit(TextNode& node) {
    // 文本节点不生成 JavaScript
}

void JavaScriptGenerator::visit(CommentNode& node) {
    // 注释节点不生成 JavaScript
}

void JavaScriptGenerator::visit(TemplateNode& node) {
    // 模板节点不生成 JavaScript
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void JavaScriptGenerator::visit(CustomNode& node) {
    // 自定义节点不生成 JavaScript
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void JavaScriptGenerator::visit(StyleNode& node) {
    // 样式节点不生成 JavaScript
}

void JavaScriptGenerator::visit(ScriptNode& node) {
    // 脚本节点生成 JavaScript 代码
    if (node.hasAttribute("content")) {
        m_output += generateJavaScriptCode(node.getAttribute("content")) + "\n";
    }
}

void JavaScriptGenerator::visit(OriginNode& node) {
    // 原始嵌入节点直接输出 JavaScript 内容
    if (node.getOriginType() == "@JavaScript" && node.hasAttribute("content")) {
        m_output += node.getAttribute("content") + "\n";
    }
}

void JavaScriptGenerator::visit(ImportNode& node) {
    // 导入节点不直接生成 JavaScript
}

void JavaScriptGenerator::visit(ConfigNode& node) {
    // 配置节点不生成 JavaScript
}

void JavaScriptGenerator::visit(NamespaceNode& node) {
    // 命名空间节点不直接生成 JavaScript
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void JavaScriptGenerator::visit(OperatorNode& node) {
    // 操作符节点不生成 JavaScript
}

void JavaScriptGenerator::processScriptBlocks() {
    // 处理脚本块
}

void JavaScriptGenerator::processCHTLJSCode() {
    // 处理 CHTL JS 代码
}

std::string JavaScriptGenerator::generateJavaScriptCode(const std::string& code) {
    return code;
}

std::string JavaScriptGenerator::generateCHTLJSCode(const std::string& code) {
    // 这里需要实现 CHTL JS 到 JavaScript 的转换
    return code;
}

// CHTLGenerator 实现
CHTLGenerator::CHTLGenerator(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    m_htmlGenerator = std::make_unique<HTMLGenerator>(context);
    m_cssGenerator = std::make_unique<CSSGenerator>(context);
    m_jsGenerator = std::make_unique<JavaScriptGenerator>(context);
}

CHTLGenerator::GeneratedCode CHTLGenerator::generate(std::shared_ptr<BaseNode> root) {
    GeneratedCode result;
    
    result.html = generateHTML(root);
    result.css = generateCSS(root);
    result.javascript = generateJavaScript(root);
    
    return result;
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<BaseNode> root) {
    return m_htmlGenerator->generate(root);
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<BaseNode> root) {
    return m_cssGenerator->generate(root);
}

std::string CHTLGenerator::generateJavaScript(std::shared_ptr<BaseNode> root) {
    return m_jsGenerator->generate(root);
}

} // namespace CHTL