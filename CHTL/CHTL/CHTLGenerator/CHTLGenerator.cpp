#include "CHTLGenerator.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLGenerator::CHTLGenerator() : currentIndent(0) {
    updateIndent();
}

CHTLGenerator::CHTLGenerator(const GeneratorOptions& opts) 
    : options(opts), currentIndent(0) {
    updateIndent();
}

void CHTLGenerator::updateIndent() {
    currentIndentStr = "";
    for (int i = 0; i < currentIndent; ++i) {
        if (options.useTabs) {
            currentIndentStr += "\t";
        } else {
            currentIndentStr += options.indentChar;
        }
    }
}

void CHTLGenerator::addIndent() {
    currentIndent += options.indentSize;
    updateIndent();
}

void CHTLGenerator::removeIndent() {
    currentIndent = std::max(0, currentIndent - options.indentSize);
    updateIndent();
}

void CHTLGenerator::addLine(const std::string& line) {
    if (options.minifyOutput) {
        outputLines.push_back(line);
    } else {
        outputLines.push_back(currentIndentStr + line);
    }
}

void CHTLGenerator::addContent(const std::string& content) {
    if (options.minifyOutput) {
        outputLines.push_back(content);
    } else {
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
            outputLines.push_back(currentIndentStr + line);
        }
    }
}

std::string CHTLGenerator::generate(std::shared_ptr<CHTLASTNode> ast) {
    resetOutput();
    
    if (!ast) {
        return "";
    }
    
    std::string result = generateHTML(ast);
    
    if (options.addDoctype && options.format == OutputFormat::HTML) {
        result = addDoctype() + "\n" + result;
    }
    
    if (options.format == OutputFormat::MINIFIED) {
        result = minifyOutput(result);
    } else if (options.format == OutputFormat::PRETTY) {
        result = formatOutput(result);
    }
    
    return result;
}

std::string CHTLGenerator::generate(std::shared_ptr<CHTLASTNode> ast, const GeneratorOptions& opts) {
    GeneratorOptions oldOptions = options;
    options = opts;
    std::string result = generate(ast);
    options = oldOptions;
    return result;
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<CHTLASTNode> node) {
    if (!node) {
        return "";
    }
    
    switch (node->type) {
        case CHTLASTNodeType::PROGRAM: {
            std::string result;
            for (const auto& child : node->children) {
                result += generateHTML(child);
            }
            return result;
        }
        
        case CHTLASTNodeType::HTML_ELEMENT: {
            auto element = std::dynamic_pointer_cast<HTMLElementNode>(node);
            return generateHTMLElement(element);
        }
        
        case CHTLASTNodeType::TEXT_NODE: {
            auto textNode = std::dynamic_pointer_cast<TextNode>(node);
            return generateTextNode(textNode);
        }
        
        case CHTLASTNodeType::STYLE_BLOCK: {
            auto styleBlock = std::dynamic_pointer_cast<StyleBlockNode>(node);
            return generateStyleBlock(styleBlock);
        }
        
        case CHTLASTNodeType::SCRIPT_BLOCK: {
            auto scriptBlock = std::dynamic_pointer_cast<ScriptBlockNode>(node);
            return generateScriptBlock(scriptBlock);
        }
        
        case CHTLASTNodeType::TEMPLATE_DECLARATION: {
            auto templateNode = std::dynamic_pointer_cast<TemplateDeclarationNode>(node);
            return generateTemplateDeclaration(templateNode);
        }
        
        case CHTLASTNodeType::CUSTOM_DECLARATION: {
            auto customNode = std::dynamic_pointer_cast<CustomDeclarationNode>(node);
            return generateCustomDeclaration(customNode);
        }
        
        case CHTLASTNodeType::ORIGIN_NODE: {
            auto originNode = std::dynamic_pointer_cast<OriginNode>(node);
            return generateOriginNode(originNode);
        }
        
        case CHTLASTNodeType::IMPORT_NODE: {
            auto importNode = std::dynamic_pointer_cast<ImportNode>(node);
            return generateImportNode(importNode);
        }
        
        case CHTLASTNodeType::NAMESPACE_NODE: {
            auto namespaceNode = std::dynamic_pointer_cast<NamespaceNode>(node);
            return generateNamespaceNode(namespaceNode);
        }
        
        case CHTLASTNodeType::CONFIGURATION_NODE: {
            auto configNode = std::dynamic_pointer_cast<ConfigurationNode>(node);
            return generateConfigurationNode(configNode);
        }
        
        case CHTLASTNodeType::INFO_NODE: {
            auto infoNode = std::dynamic_pointer_cast<InfoNode>(node);
            return generateInfoNode(infoNode);
        }
        
        case CHTLASTNodeType::EXPORT_NODE: {
            auto exportNode = std::dynamic_pointer_cast<ExportNode>(node);
            return generateExportNode(exportNode);
        }
        
        case CHTLASTNodeType::DELETE_NODE: {
            return generateDeleteNode(node);
        }
        
        case CHTLASTNodeType::INSERT_NODE: {
            return generateInsertNode(node);
        }
        
        case CHTLASTNodeType::INHERIT_NODE: {
            return generateInheritNode(node);
        }
        
        case CHTLASTNodeType::EXCEPT_NODE: {
            return generateExceptNode(node);
        }
        
        default:
            return "";
    }
}

std::string CHTLGenerator::generateHTMLElement(std::shared_ptr<HTMLElementNode> element) {
    if (!element) {
        return "";
    }
    
    std::ostringstream html;
    
    // 开始标签
    html << "<" << element->tagName;
    
    // 添加属性
    for (const auto& attr : element->attributes) {
        if (attr) {
            html << " " << generateAttribute(attr);
        }
    }
    
    if (element->isSelfClosing) {
        if (options.format == OutputFormat::XHTML) {
            html << " />";
        } else {
            html << ">";
        }
    } else {
        html << ">";
        
        // 添加子元素
        for (const auto& child : element->children) {
            if (child) {
                html << generateHTML(child);
            }
        }
        
        // 结束标签
        html << "</" << element->tagName << ">";
    }
    
    return html.str();
}

std::string CHTLGenerator::generateTextNode(std::shared_ptr<TextNode> textNode) {
    if (!textNode) {
        return "";
    }
    
    if (textNode->isRaw) {
        return textNode->content;
    }
    
    return escapeHTML(textNode->content);
}

std::string CHTLGenerator::generateAttribute(std::shared_ptr<AttributeNode> attribute) {
    if (!attribute) {
        return "";
    }
    
    if (attribute->hasValue) {
        return attribute->name + "=\"" + escapeHTML(attribute->value) + "\"";
    } else {
        return attribute->name;
    }
}

std::string CHTLGenerator::generateStyleBlock(std::shared_ptr<StyleBlockNode> styleBlock) {
    if (!styleBlock) {
        return "";
    }
    
    std::ostringstream css;
    
    if (styleBlock->isGlobal) {
        css << "<style";
        if (!options.charset.empty()) {
            css << " type=\"text/css\"";
        }
        css << ">\n";
    }
    
    for (const auto& rule : styleBlock->rules) {
        if (rule) {
            css << generateStyleRule(rule) << "\n";
        }
    }
    
    if (styleBlock->isGlobal) {
        css << "</style>";
    }
    
    return css.str();
}

std::string CHTLGenerator::generateStyleRule(std::shared_ptr<StyleRuleNode> styleRule) {
    if (!styleRule) {
        return "";
    }
    
    std::ostringstream css;
    
    if (styleRule->selector) {
        css << styleRule->selector->toHTML() << " {\n";
    }
    
    for (const auto& prop : styleRule->properties) {
        if (prop) {
            css << "    " << generateStyleProperty(prop) << ";\n";
        }
    }
    
    css << "}";
    
    return css.str();
}

std::string CHTLGenerator::generateStyleProperty(std::shared_ptr<StylePropertyNode> property) {
    if (!property) {
        return "";
    }
    
    std::ostringstream css;
    css << property->name << ": " << property->value;
    if (property->isImportant) {
        css << " !important";
    }
    return css.str();
}

std::string CHTLGenerator::generateScriptBlock(std::shared_ptr<ScriptBlockNode> scriptBlock) {
    if (!scriptBlock) {
        return "";
    }
    
    std::ostringstream js;
    
    if (scriptBlock->isInline) {
        js << "<script type=\"" << scriptBlock->type << "\">\n";
        js << scriptBlock->content << "\n";
        js << "</script>";
    } else {
        js << "<script type=\"" << scriptBlock->type << "\" src=\"" 
           << scriptBlock->content << "\"></script>";
    }
    
    return js.str();
}

std::string CHTLGenerator::generateTemplateDeclaration(std::shared_ptr<TemplateDeclarationNode> templateNode) {
    if (!templateNode) {
        return "";
    }
    
    // 模板声明不直接生成HTML，但可以用于调试
    if (options.includeComments) {
        std::ostringstream html;
        html << "<!-- Template: " << templateNode->templateType 
             << " " << templateNode->name << " -->";
        return html.str();
    }
    
    return "";
}

std::string CHTLGenerator::generateCustomDeclaration(std::shared_ptr<CustomDeclarationNode> customNode) {
    if (!customNode) {
        return "";
    }
    
    // 自定义声明不直接生成HTML，但可以用于调试
    if (options.includeComments) {
        std::ostringstream html;
        html << "<!-- Custom: " << customNode->customType 
             << " " << customNode->name << " -->";
        return html.str();
    }
    
    return "";
}

std::string CHTLGenerator::generateOriginNode(std::shared_ptr<OriginNode> originNode) {
    if (!originNode) {
        return "";
    }
    
    if (originNode->originType == "@Html") {
        return originNode->content;
    } else if (originNode->originType == "@Style") {
        return "<style>\n" + originNode->content + "\n</style>";
    } else if (originNode->originType == "@JavaScript") {
        return "<script>\n" + originNode->content + "\n</script>";
    }
    
    return "";
}

std::string CHTLGenerator::generateImportNode(std::shared_ptr<ImportNode> importNode) {
    if (!importNode) {
        return "";
    }
    
    if (importNode->importType == "@Html") {
        if (options.includeComments) {
            return "<!-- Import HTML: " + importNode->path + " -->";
        }
    } else if (importNode->importType == "@Style") {
        return "<link rel=\"stylesheet\" href=\"" + importNode->path + "\">";
    } else if (importNode->importType == "@JavaScript") {
        return "<script src=\"" + importNode->path + "\"></script>";
    }
    
    if (options.includeComments) {
        return "<!-- Import: " + importNode->importType + " " + importNode->path + " -->";
    }
    
    return "";
}

std::string CHTLGenerator::generateNamespaceNode(std::shared_ptr<NamespaceNode> namespaceNode) {
    if (!namespaceNode) {
        return "";
    }
    
    if (options.includeComments) {
        std::ostringstream html;
        html << "<!-- Namespace: " << namespaceNode->name << " -->";
        return html.str();
    }
    
    return "";
}

std::string CHTLGenerator::generateConfigurationNode(std::shared_ptr<ConfigurationNode> configNode) {
    if (!configNode) {
        return "";
    }
    
    if (options.includeComments) {
        std::ostringstream html;
        html << "<!-- Configuration: " << configNode->name << " -->";
        return html.str();
    }
    
    return "";
}

std::string CHTLGenerator::generateInfoNode(std::shared_ptr<InfoNode> infoNode) {
    if (!infoNode) {
        return "";
    }
    
    if (options.includeComments) {
        std::ostringstream html;
        html << "<!-- Info: ";
        for (const auto& pair : infoNode->info) {
            html << pair.first << "=" << pair.second << " ";
        }
        html << "-->";
        return html.str();
    }
    
    return "";
}

std::string CHTLGenerator::generateExportNode(std::shared_ptr<ExportNode> exportNode) {
    if (!exportNode) {
        return "";
    }
    
    if (options.includeComments) {
        std::ostringstream html;
        html << "<!-- Export: ";
        for (const auto& exportName : exportNode->exports) {
            html << exportName << " ";
        }
        html << "-->";
        return html.str();
    }
    
    return "";
}

std::string CHTLGenerator::generateDeleteNode(std::shared_ptr<CHTLASTNode> deleteNode) {
    if (!deleteNode) {
        return "";
    }
    
    if (options.includeComments) {
        std::ostringstream html;
        html << "<!-- Delete: ";
        for (const auto& child : deleteNode->children) {
            if (child) {
                html << child->toHTML() << " ";
            }
        }
        html << "-->";
        return html.str();
    }
    
    return "";
}

std::string CHTLGenerator::generateInsertNode(std::shared_ptr<CHTLASTNode> insertNode) {
    if (!insertNode) {
        return "";
    }
    
    if (options.includeComments) {
        std::ostringstream html;
        html << "<!-- Insert: ";
        for (const auto& child : insertNode->children) {
            if (child) {
                html << child->toHTML() << " ";
            }
        }
        html << "-->";
        return html.str();
    }
    
    return "";
}

std::string CHTLGenerator::generateInheritNode(std::shared_ptr<CHTLASTNode> inheritNode) {
    if (!inheritNode) {
        return "";
    }
    
    if (options.includeComments) {
        std::ostringstream html;
        html << "<!-- Inherit: ";
        for (const auto& child : inheritNode->children) {
            if (child) {
                html << child->toHTML() << " ";
            }
        }
        html << "-->";
        return html.str();
    }
    
    return "";
}

std::string CHTLGenerator::generateExceptNode(std::shared_ptr<CHTLASTNode> exceptNode) {
    if (!exceptNode) {
        return "";
    }
    
    if (options.includeComments) {
        std::ostringstream html;
        html << "<!-- Except: ";
        for (const auto& child : exceptNode->children) {
            if (child) {
                html << child->toHTML() << " ";
            }
        }
        html << "-->";
        return html.str();
    }
    
    return "";
}

std::string CHTLGenerator::escapeHTML(const std::string& text) const {
    std::string result = text;
    
    // 转义HTML特殊字符
    size_t pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    pos = 0;
    while ((pos = result.find("<", pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = result.find(">", pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    pos = 0;
    while ((pos = result.find("'", pos)) != std::string::npos) {
        result.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return result;
}

std::string CHTLGenerator::escapeCSS(const std::string& text) const {
    // CSS转义逻辑
    return text; // 简化实现
}

std::string CHTLGenerator::escapeJS(const std::string& text) const {
    // JavaScript转义逻辑
    return text; // 简化实现
}

std::string CHTLGenerator::formatOutput(const std::string& content) const {
    // 美化输出逻辑
    return content; // 简化实现
}

std::string CHTLGenerator::minifyOutput(const std::string& content) const {
    std::string result = content;
    
    // 移除多余空白
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    
    // 移除注释
    if (!options.includeComments) {
        result = std::regex_replace(result, std::regex("<!--.*?-->"), "");
    }
    
    // 移除换行
    result = std::regex_replace(result, std::regex("\\n\\s*"), "");
    
    return result;
}

std::string CHTLGenerator::addDoctype() const {
    if (options.format == OutputFormat::XHTML) {
        return "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">";
    } else {
        return "<!DOCTYPE html>";
    }
}

std::string CHTLGenerator::addMetaTags() const {
    std::ostringstream meta;
    meta << "<meta charset=\"" << options.charset << "\">\n";
    meta << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
    return meta.str();
}

std::string CHTLGenerator::processTemplate(const std::string& templateName, 
                                          const std::unordered_map<std::string, std::string>& params) const {
    auto it = templates.find(templateName);
    if (it != templates.end()) {
        // 处理模板参数替换
        return it->second->toHTML();
    }
    return "";
}

std::string CHTLGenerator::processCustomElement(const std::string& elementName,
                                               const std::unordered_map<std::string, std::string>& params) const {
    auto it = customElements.find(elementName);
    if (it != customElements.end()) {
        // 处理自定义元素参数替换
        return it->second->toHTML();
    }
    return "";
}

std::string CHTLGenerator::processVariable(const std::string& varName) const {
    auto it = variables.find(varName);
    if (it != variables.end()) {
        return it->second;
    }
    return "";
}

bool CHTLGenerator::isSelfClosingElement(const std::string& tagName) const {
    static const std::vector<std::string> selfClosingElements = {
        "img", "br", "hr", "input", "meta", "link", "area", "base", "col", "embed",
        "source", "track", "wbr"
    };
    
    return std::find(selfClosingElements.begin(), selfClosingElements.end(), tagName) != selfClosingElements.end();
}

bool CHTLGenerator::isInlineElement(const std::string& tagName) const {
    static const std::vector<std::string> inlineElements = {
        "a", "span", "strong", "em", "b", "i", "u", "small", "sub", "sup",
        "code", "kbd", "samp", "var", "cite", "abbr", "acronym", "dfn"
    };
    
    return std::find(inlineElements.begin(), inlineElements.end(), tagName) != inlineElements.end();
}

std::string CHTLGenerator::getIndentString() const {
    return currentIndentStr;
}

void CHTLGenerator::resetOutput() {
    outputLines.clear();
    currentIndent = 0;
    updateIndent();
}

void CHTLGenerator::setOptions(const GeneratorOptions& opts) {
    options = opts;
    updateIndent();
}

GeneratorOptions CHTLGenerator::getOptions() const {
    return options;
}

void CHTLGenerator::setOutputFormat(OutputFormat format) {
    options.format = format;
}

void CHTLGenerator::setMinifyOutput(bool minify) {
    options.minifyOutput = minify;
}

void CHTLGenerator::setIncludeComments(bool include) {
    options.includeComments = include;
}

void CHTLGenerator::setPreserveWhitespace(bool preserve) {
    options.preserveWhitespace = preserve;
}

void CHTLGenerator::setIndentSize(int size) {
    options.indentSize = size;
    updateIndent();
}

void CHTLGenerator::setUseTabs(bool useTabs) {
    options.useTabs = useTabs;
    updateIndent();
}

void CHTLGenerator::setAddDoctype(bool add) {
    options.addDoctype = add;
}

void CHTLGenerator::setCharset(const std::string& charset) {
    options.charset = charset;
}

void CHTLGenerator::setLang(const std::string& lang) {
    options.lang = lang;
}

void CHTLGenerator::addTemplate(const std::string& name, std::shared_ptr<CHTLASTNode> templateNode) {
    if (templateNode) {
        templates[name] = templateNode;
    }
}

void CHTLGenerator::removeTemplate(const std::string& name) {
    templates.erase(name);
}

bool CHTLGenerator::hasTemplate(const std::string& name) const {
    return templates.find(name) != templates.end();
}

std::shared_ptr<CHTLASTNode> CHTLGenerator::getTemplate(const std::string& name) const {
    auto it = templates.find(name);
    if (it != templates.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLGenerator::addCustomElement(const std::string& name, std::shared_ptr<CHTLASTNode> element) {
    if (element) {
        customElements[name] = element;
    }
}

void CHTLGenerator::removeCustomElement(const std::string& name) {
    customElements.erase(name);
}

bool CHTLGenerator::hasCustomElement(const std::string& name) const {
    return customElements.find(name) != customElements.end();
}

std::shared_ptr<CHTLASTNode> CHTLGenerator::getCustomElement(const std::string& name) const {
    auto it = customElements.find(name);
    if (it != customElements.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLGenerator::setVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

void CHTLGenerator::removeVariable(const std::string& name) {
    variables.erase(name);
}

bool CHTLGenerator::hasVariable(const std::string& name) const {
    return variables.find(name) != variables.end();
}

std::string CHTLGenerator::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    return "";
}

void CHTLGenerator::addNamespace(const std::string& name, const std::string& prefix) {
    namespaces[name] = prefix;
}

void CHTLGenerator::removeNamespace(const std::string& name) {
    namespaces.erase(name);
}

bool CHTLGenerator::hasNamespace(const std::string& name) const {
    return namespaces.find(name) != namespaces.end();
}

std::string CHTLGenerator::getNamespacePrefix(const std::string& name) const {
    auto it = namespaces.find(name);
    if (it != namespaces.end()) {
        return it->second;
    }
    return "";
}

std::vector<std::string> CHTLGenerator::getOutputLines() const {
    return outputLines;
}

std::string CHTLGenerator::getOutput() const {
    std::ostringstream oss;
    for (const auto& line : outputLines) {
        oss << line << "\n";
    }
    return oss.str();
}

void CHTLGenerator::clearOutput() {
    resetOutput();
}

void CHTLGenerator::printTemplates() const {
    std::cout << "=== 模板 ===" << std::endl;
    for (const auto& pair : templates) {
        std::cout << pair.first << " -> " << pair.second->toString() << std::endl;
    }
    std::cout << "===========" << std::endl;
}

void CHTLGenerator::printCustomElements() const {
    std::cout << "=== 自定义元素 ===" << std::endl;
    for (const auto& pair : customElements) {
        std::cout << pair.first << " -> " << pair.second->toString() << std::endl;
    }
    std::cout << "===============" << std::endl;
}

void CHTLGenerator::printVariables() const {
    std::cout << "=== 变量 ===" << std::endl;
    for (const auto& pair : variables) {
        std::cout << pair.first << " = " << pair.second << std::endl;
    }
    std::cout << "===========" << std::endl;
}

void CHTLGenerator::printNamespaces() const {
    std::cout << "=== 命名空间 ===" << std::endl;
    for (const auto& pair : namespaces) {
        std::cout << pair.first << " -> " << pair.second << std::endl;
    }
    std::cout << "=============" << std::endl;
}

void CHTLGenerator::printOptions() const {
    std::cout << "=== 生成器选项 ===" << std::endl;
    std::cout << "输出格式: " << static_cast<int>(options.format) << std::endl;
    std::cout << "包含注释: " << (options.includeComments ? "是" : "否") << std::endl;
    std::cout << "压缩输出: " << (options.minifyOutput ? "是" : "否") << std::endl;
    std::cout << "保持空白: " << (options.preserveWhitespace ? "是" : "否") << std::endl;
    std::cout << "缩进大小: " << options.indentSize << std::endl;
    std::cout << "使用制表符: " << (options.useTabs ? "是" : "否") << std::endl;
    std::cout << "添加DOCTYPE: " << (options.addDoctype ? "是" : "否") << std::endl;
    std::cout << "字符集: " << options.charset << std::endl;
    std::cout << "语言: " << options.lang << std::endl;
    std::cout << "===============" << std::endl;
}

} // namespace CHTL