#include "CHTL/CHTLGenerator.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// CHTLGenerator 实现
CHTLGenerator::CHTLGenerator(std::ostream& output)
    : output_(output) {
}

void CHTLGenerator::setConfig(const ConfigOptions& config) {
    config_ = config;
}

const ErrorList& CHTLGenerator::getErrors() const {
    return errors_;
}

void CHTLGenerator::increaseIndent() {
    indentLevel_++;
}

void CHTLGenerator::decreaseIndent() {
    if (indentLevel_ > 0) {
        indentLevel_--;
    }
}

void CHTLGenerator::writeIndent() {
    for (int i = 0; i < indentLevel_; ++i) {
        output_ << indentString_;
    }
}

void CHTLGenerator::addError(const String& message, const Position& pos) {
    errors_.emplace_back(message, pos, "error");
}

String CHTLGenerator::escapeHtml(const String& str) const {
    String result;
    for (char c : str) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c; break;
        }
    }
    return result;
}

String CHTLGenerator::escapeCss(const String& str) const {
    String result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\'': result += "\\'"; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

String CHTLGenerator::escapeJs(const String& str) const {
    String result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\'': result += "\\'"; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            default: result += c; break;
        }
    }
    return result;
}

String CHTLGenerator::formatAttribute(const Attribute& attr) const {
    String result = attr.name;
    if (!attr.value.empty()) {
        result += "=";
        if (attr.isQuoted) {
            result += "\"" + escapeHtml(attr.value) + "\"";
        } else {
            result += escapeHtml(attr.value);
        }
    }
    return result;
}

String CHTLGenerator::formatAttributes(const AttributeList& attrs) const {
    String result;
    for (size_t i = 0; i < attrs.size(); ++i) {
        if (i > 0) result += " ";
        result += formatAttribute(attrs[i]);
    }
    return result;
}

// HTMLGenerator 实现
HTMLGenerator::HTMLGenerator(std::ostream& output)
    : CHTLGenerator(output) {
}

void HTMLGenerator::generate(std::shared_ptr<CHTLNode> root) {
    if (!root) return;
    
    // 生成 HTML 文档
    output_ << "<!DOCTYPE html>\n";
    output_ << "<html>\n";
    
    // 处理根节点的子节点
    for (const auto& child : root->getChildren()) {
        child->accept(*this);
    }
    
    output_ << "</html>\n";
}

void HTMLGenerator::visit(TextNode& node) {
    output_ << escapeHtml(node.getText());
}

void HTMLGenerator::visit(ElementNode& node) {
    generateElement(node);
}

void HTMLGenerator::visit(CommentNode& node) {
    // HTML 生成器不处理注释
}

void HTMLGenerator::visit(StyleNode& node) {
    generateStyle(node);
}

void HTMLGenerator::visit(ScriptNode& node) {
    generateScript(node);
}

void HTMLGenerator::visit(TemplateStyleNode& node) {
    generateTemplate(node);
}

void HTMLGenerator::visit(TemplateElementNode& node) {
    generateTemplate(node);
}

void HTMLGenerator::visit(TemplateVarNode& node) {
    generateTemplate(node);
}

void HTMLGenerator::visit(CustomStyleNode& node) {
    generateCustom(node);
}

void HTMLGenerator::visit(CustomElementNode& node) {
    generateCustom(node);
}

void HTMLGenerator::visit(CustomVarNode& node) {
    generateCustom(node);
}

void HTMLGenerator::visit(OriginNode& node) {
    generateOrigin(node);
}

void HTMLGenerator::visit(ImportNode& node) {
    generateImport(node);
}

void HTMLGenerator::visit(ConfigNode& node) {
    // HTML 生成器不处理配置
}

void HTMLGenerator::visit(NamespaceNode& node) {
    // HTML 生成器不处理命名空间
}

void HTMLGenerator::visit(OperatorNode& node) {
    // HTML 生成器不处理操作符
}

void HTMLGenerator::generateElement(ElementNode& node) {
    String tagName = node.getTagName();
    if (tagName.empty()) return;
    
    // 检查是否为自闭合标签
    if (isVoidElement(tagName)) {
        output_ << "<" << tagName;
        String attrs = formatAttributes(node.getAttributes());
        if (!attrs.empty()) {
            output_ << " " << attrs;
        }
        output_ << " />\n";
        return;
    }
    
    // 开始标签
    output_ << "<" << tagName;
    String attrs = formatAttributes(node.getAttributes());
    if (!attrs.empty()) {
        output_ << " " << attrs;
    }
    output_ << ">";
    
    // 处理子节点
    if (!node.getChildren().empty()) {
        if (isBlockElement(tagName)) {
            output_ << "\n";
            increaseIndent();
        }
        
        for (const auto& child : node.getChildren()) {
            if (isBlockElement(tagName)) {
                writeIndent();
            }
            child->accept(*this);
            if (isBlockElement(tagName)) {
                output_ << "\n";
            }
        }
        
        if (isBlockElement(tagName)) {
            decreaseIndent();
            writeIndent();
        }
    }
    
    // 结束标签
    output_ << "</" << tagName << ">";
    if (isBlockElement(tagName)) {
        output_ << "\n";
    }
}

void HTMLGenerator::generateText(TextNode& node) {
    output_ << escapeHtml(node.getText());
}

void HTMLGenerator::generateComment(CommentNode& node) {
    // HTML 生成器不处理注释
}

void HTMLGenerator::generateStyle(StyleNode& node) {
    output_ << "<style";
    String attrs = formatAttributes(node.getAttributes());
    if (!attrs.empty()) {
        output_ << " " << attrs;
    }
    output_ << ">\n";
    
    String content = node.getStyleContent();
    if (!content.empty()) {
        output_ << content << "\n";
    }
    
    output_ << "</style>\n";
}

void HTMLGenerator::generateScript(ScriptNode& node) {
    output_ << "<script";
    String attrs = formatAttributes(node.getAttributes());
    if (!attrs.empty()) {
        output_ << " " << attrs;
    }
    output_ << ">\n";
    
    String content = node.getScriptContent();
    if (!content.empty()) {
        output_ << content << "\n";
    }
    
    output_ << "</script>\n";
}

void HTMLGenerator::generateTemplate(TemplateNode& node) {
    // 处理模板
    processTemplate(node);
}

void HTMLGenerator::generateCustom(CustomNode& node) {
    // 处理自定义
    processCustom(node);
}

void HTMLGenerator::generateOrigin(OriginNode& node) {
    String type = node.getOriginType();
    String content = node.getOriginContent();
    
    if (type == "@Html") {
        output_ << content;
    } else if (type == "@Style") {
        output_ << "<style>\n" << content << "\n</style>\n";
    } else if (type == "@JavaScript") {
        output_ << "<script>\n" << content << "\n</script>\n";
    }
}

void HTMLGenerator::generateImport(ImportNode& node) {
    // 处理导入
    processImport(node);
}

bool HTMLGenerator::isVoidElement(const String& tagName) const {
    static const StringSet voidElements = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return voidElements.find(tagName) != voidElements.end();
}

bool HTMLGenerator::isBlockElement(const String& tagName) const {
    static const StringSet blockElements = {
        "div", "p", "h1", "h2", "h3", "h4", "h5", "h6", "ul", "ol", "li",
        "table", "tr", "td", "th", "form", "section", "article", "header",
        "footer", "nav", "aside", "main", "figure", "figcaption"
    };
    
    return blockElements.find(tagName) != blockElements.end();
}

bool HTMLGenerator::isInlineElement(const String& tagName) const {
    static const StringSet inlineElements = {
        "span", "a", "strong", "em", "code", "small", "mark", "del", "ins",
        "sub", "sup", "q", "cite", "abbr", "time", "data", "var", "samp",
        "kbd", "b", "i", "u", "s", "strike"
    };
    
    return inlineElements.find(tagName) != inlineElements.end();
}

String HTMLGenerator::formatTagName(const String& name) const {
    return name;
}

String HTMLGenerator::formatAttributeValue(const String& value) const {
    return escapeHtml(value);
}

void HTMLGenerator::processTemplate(TemplateNode& node) {
    // 处理模板逻辑
    templates_[node.getTemplateName()] = std::make_shared<TemplateNode>(node);
}

void HTMLGenerator::processCustom(CustomNode& node) {
    // 处理自定义逻辑
    customs_[node.getCustomName()] = std::make_shared<CustomNode>(node);
}

void HTMLGenerator::processInheritance(TemplateNode& node) {
    // 处理模板继承
    for (const auto& inherit : node.getInherits()) {
        auto parent = templates_.find(inherit);
        if (parent != templates_.end()) {
            // 处理继承逻辑
        }
    }
}

void HTMLGenerator::processSpecialization(CustomNode& node) {
    // 处理自定义特例化
    for (const auto& del : node.getDeletes()) {
        // 处理删除逻辑
    }
    
    for (const auto& ins : node.getInserts()) {
        // 处理插入逻辑
    }
}

String HTMLGenerator::replaceVariables(const String& content) const {
    String result = content;
    
    for (const auto& var : variables_) {
        String placeholder = "{{" + var.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != String::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos += var.second.length();
        }
    }
    
    return result;
}

String HTMLGenerator::getVariableValue(const String& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

String HTMLGenerator::resolveNamespace(const String& name) const {
    // 解析命名空间
    return name;
}

void HTMLGenerator::processNamespace(NamespaceNode& node) {
    // 处理命名空间
    namespaces_[node.getNamespaceName()] = std::make_shared<NamespaceNode>(node);
}

void HTMLGenerator::processImport(ImportNode& node) {
    // 处理导入
    imports_.push_back(std::make_shared<ImportNode>(node));
}

void HTMLGenerator::processConfig(ConfigNode& node) {
    // 处理配置
    config_ = node.getConfigOptions();
}

// CSSGenerator 实现
CSSGenerator::CSSGenerator(std::ostream& output)
    : CHTLGenerator(output) {
}

void CSSGenerator::generate(std::shared_ptr<CHTLNode> root) {
    if (!root) return;
    
    // 生成 CSS
    for (const auto& child : root->getChildren()) {
        child->accept(*this);
    }
}

void CSSGenerator::visit(TextNode& node) {
    // CSS 生成器不处理文本节点
}

void CSSGenerator::visit(ElementNode& node) {
    // CSS 生成器不处理元素节点
}

void CSSGenerator::visit(CommentNode& node) {
    // CSS 生成器不处理注释节点
}

void CSSGenerator::visit(StyleNode& node) {
    generateStyle(node);
}

void CSSGenerator::visit(ScriptNode& node) {
    // CSS 生成器不处理脚本节点
}

void CSSGenerator::visit(TemplateStyleNode& node) {
    generateTemplateStyle(node);
}

void CSSGenerator::visit(TemplateElementNode& node) {
    // CSS 生成器不处理元素模板
}

void CSSGenerator::visit(TemplateVarNode& node) {
    // CSS 生成器不处理变量模板
}

void CSSGenerator::visit(CustomStyleNode& node) {
    generateCustomStyle(node);
}

void CSSGenerator::visit(CustomElementNode& node) {
    // CSS 生成器不处理自定义元素
}

void CSSGenerator::visit(CustomVarNode& node) {
    // CSS 生成器不处理自定义变量
}

void CSSGenerator::visit(OriginNode& node) {
    generateOriginStyle(node);
}

void CSSGenerator::visit(ImportNode& node) {
    // CSS 生成器不处理导入节点
}

void CSSGenerator::visit(ConfigNode& node) {
    // CSS 生成器不处理配置节点
}

void CSSGenerator::visit(NamespaceNode& node) {
    // CSS 生成器不处理命名空间节点
}

void CSSGenerator::visit(OperatorNode& node) {
    // CSS 生成器不处理操作符节点
}

void CSSGenerator::generateStyle(StyleNode& node) {
    String content = node.getStyleContent();
    if (!content.empty()) {
        output_ << content << "\n";
    }
}

void CSSGenerator::generateTemplateStyle(TemplateStyleNode& node) {
    // 处理样式组模板
    templates_[node.getTemplateName()] = std::make_shared<TemplateStyleNode>(node);
}

void CSSGenerator::generateCustomStyle(CustomStyleNode& node) {
    // 处理自定义样式组
    customs_[node.getCustomName()] = std::make_shared<CustomStyleNode>(node);
}

void CSSGenerator::generateOriginStyle(OriginNode& node) {
    String content = node.getOriginContent();
    if (!content.empty()) {
        output_ << content << "\n";
    }
}

String CSSGenerator::formatCssProperty(const String& name, const String& value) const {
    return name + ": " + formatCssValue(value) + ";";
}

String CSSGenerator::formatCssSelector(const String& selector) const {
    return selector;
}

String CSSGenerator::formatCssValue(const String& value) const {
    return escapeCss(value);
}

void CSSGenerator::processSelectors(StyleNode& node) {
    // 处理选择器
}

void CSSGenerator::processClassSelectors(StyleNode& node) {
    // 处理类选择器
}

void CSSGenerator::processIdSelectors(StyleNode& node) {
    // 处理 ID 选择器
}

void CSSGenerator::processPseudoSelectors(StyleNode& node) {
    // 处理伪选择器
}

String CSSGenerator::processConditionalExpression(const String& expr) const {
    // 处理条件表达式
    return expr;
}

String CSSGenerator::processLogicalExpression(const String& expr) const {
    // 处理逻辑表达式
    return expr;
}

String CSSGenerator::processComparisonExpression(const String& expr) const {
    // 处理比较表达式
    return expr;
}

String CSSGenerator::processArithmeticExpression(const String& expr) const {
    // 处理算术表达式
    return expr;
}

String CSSGenerator::resolveVariable(const String& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

void CSSGenerator::processVariables(TemplateVarNode& node) {
    // 处理变量
    variables_[node.getTemplateName()] = node.getText();
}

void CSSGenerator::processInheritance(TemplateStyleNode& node) {
    // 处理样式组继承
}

void CSSGenerator::processSpecialization(CustomStyleNode& node) {
    // 处理自定义样式组特例化
}

String CSSGenerator::resolveNamespace(const String& name) const {
    // 解析命名空间
    return name;
}

// JSGenerator 实现
JSGenerator::JSGenerator(std::ostream& output)
    : CHTLGenerator(output) {
}

void JSGenerator::generate(std::shared_ptr<CHTLNode> root) {
    if (!root) return;
    
    // 生成 JavaScript
    for (const auto& child : root->getChildren()) {
        child->accept(*this);
    }
}

void JSGenerator::visit(TextNode& node) {
    // JS 生成器不处理文本节点
}

void JSGenerator::visit(ElementNode& node) {
    // JS 生成器不处理元素节点
}

void JSGenerator::visit(CommentNode& node) {
    // JS 生成器不处理注释节点
}

void JSGenerator::visit(StyleNode& node) {
    // JS 生成器不处理样式节点
}

void JSGenerator::visit(ScriptNode& node) {
    generateScript(node);
}

void JSGenerator::visit(TemplateStyleNode& node) {
    // JS 生成器不处理样式模板
}

void JSGenerator::visit(TemplateElementNode& node) {
    // JS 生成器不处理元素模板
}

void JSGenerator::visit(TemplateVarNode& node) {
    // JS 生成器不处理变量模板
}

void JSGenerator::visit(CustomStyleNode& node) {
    // JS 生成器不处理自定义样式
}

void JSGenerator::visit(CustomElementNode& node) {
    // JS 生成器不处理自定义元素
}

void JSGenerator::visit(CustomVarNode& node) {
    // JS 生成器不处理自定义变量
}

void JSGenerator::visit(OriginNode& node) {
    generateOriginScript(node);
}

void JSGenerator::visit(ImportNode& node) {
    // JS 生成器不处理导入节点
}

void JSGenerator::visit(ConfigNode& node) {
    // JS 生成器不处理配置节点
}

void JSGenerator::visit(NamespaceNode& node) {
    // JS 生成器不处理命名空间节点
}

void JSGenerator::visit(OperatorNode& node) {
    // JS 生成器不处理操作符节点
}

void JSGenerator::generateScript(ScriptNode& node) {
    String content = node.getScriptContent();
    if (!content.empty()) {
        output_ << content << "\n";
    }
}

void JSGenerator::generateOriginScript(OriginNode& node) {
    String content = node.getOriginContent();
    if (!content.empty()) {
        output_ << content << "\n";
    }
}

String JSGenerator::formatJsExpression(const String& expr) const {
    return escapeJs(expr);
}

String JSGenerator::formatJsFunction(const String& name, const String& params, const String& body) const {
    return "function " + name + "(" + params + ") {\n" + body + "\n}";
}

String JSGenerator::formatJsObject(const String& obj) const {
    return "{" + obj + "}";
}

String JSGenerator::formatJsArray(const String& arr) const {
    return "[" + arr + "]";
}

String JSGenerator::processSelector(const String& selector) const {
    // 处理选择器
    return selector;
}

String JSGenerator::processClassSelector(const String& className) const {
    return "." + className;
}

String JSGenerator::processIdSelector(const String& id) const {
    return "#" + id;
}

String JSGenerator::processTagSelector(const String& tagName) const {
    return tagName;
}

void JSGenerator::processEventListeners(ScriptNode& node) {
    // 处理事件监听器
}

void JSGenerator::processEventDelegation(ScriptNode& node) {
    // 处理事件委托
}

void JSGenerator::processAnimations(ScriptNode& node) {
    // 处理动画
}

void JSGenerator::processRouting(ScriptNode& node) {
    // 处理路由
}

void JSGenerator::processVirtualObjects(ScriptNode& node) {
    // 处理虚对象
}

String JSGenerator::resolveVariable(const String& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

} // namespace CHTL