#include "CHTL/CHTLGenerator.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLGenerator::CHTLGenerator(std::shared_ptr<ASTNode> ast, CHTLContext& context)
    : ast_(ast), context_(context), inStyleBlock_(false), inScriptBlock_(false), indentLevel_(0) {
}

void CHTLGenerator::generate(const std::string& outputFile) {
    // 清空输出代码
    htmlCode_.clear();
    cssCode_.clear();
    jsCode_.clear();
    
    // 生成代码
    generateNode(ast_);
    
    // 写入文件
    writeToFile(outputFile, htmlCode_);
    
    // 生成单独的CSS和JS文件
    if (!cssCode_.empty()) {
        std::string cssFile = outputFile.substr(0, outputFile.find_last_of('.')) + ".css";
        writeToFile(cssFile, cssCode_);
    }
    
    if (!jsCode_.empty()) {
        std::string jsFile = outputFile.substr(0, outputFile.find_last_of('.')) + ".js";
        writeToFile(jsFile, jsCode_);
    }
}

void CHTLGenerator::generateHTML(const std::string& outputFile) {
    generateNode(ast_);
    writeToFile(outputFile, htmlCode_);
}

void CHTLGenerator::generateCSS(const std::string& outputFile) {
    generateNode(ast_);
    writeToFile(outputFile, cssCode_);
}

void CHTLGenerator::generateJS(const std::string& outputFile) {
    generateNode(ast_);
    writeToFile(outputFile, jsCode_);
}

void CHTLGenerator::generateNode(std::shared_ptr<ASTNode> node) {
    if (!node) return;
    
    switch (node->getType()) {
        case ASTNodeType::ELEMENT:
            generateElement(std::static_pointer_cast<ElementNode>(node));
            break;
        case ASTNodeType::TEXT:
            generateText(std::static_pointer_cast<TextNode>(node));
            break;
        case ASTNodeType::STYLE_BLOCK:
            generateStyle(std::static_pointer_cast<StyleNode>(node));
            break;
        case ASTNodeType::SCRIPT_BLOCK:
            generateScript(std::static_pointer_cast<ScriptNode>(node));
            break;
        case ASTNodeType::TEMPLATE:
            generateTemplate(std::static_pointer_cast<TemplateNode>(node));
            break;
        case ASTNodeType::CUSTOM:
            generateCustom(std::static_pointer_cast<CustomNode>(node));
            break;
        case ASTNodeType::ORIGIN:
            generateOrigin(std::static_pointer_cast<OriginNode>(node));
            break;
        case ASTNodeType::IMPORT:
            generateImport(std::static_pointer_cast<ImportNode>(node));
            break;
        case ASTNodeType::NAMESPACE:
            generateNamespace(std::static_pointer_cast<NamespaceNode>(node));
            break;
        case ASTNodeType::CONFIGURATION:
            generateConfiguration(std::static_pointer_cast<ConfigurationNode>(node));
            break;
        case ASTNodeType::CONSTRAINT:
            generateConstraint(std::static_pointer_cast<ConstraintNode>(node));
            break;
        case ASTNodeType::USE:
            generateUse(std::static_pointer_cast<UseNode>(node));
            break;
        default:
            // 递归处理子节点
            for (auto& child : node->getChildren()) {
                generateNode(child);
            }
            break;
    }
}

void CHTLGenerator::generateElement(std::shared_ptr<ElementNode> element) {
    std::string tagName = element->getTagName();
    std::string attributes = generateAttributes(element->getAttributes());
    
    // 开始标签
    htmlCode_ += getIndent() + "<" + tagName;
    if (!attributes.empty()) {
        htmlCode_ += " " + attributes;
    }
    
    if (element->isSelfClosing()) {
        htmlCode_ += " />\n";
    } else {
        htmlCode_ += ">\n";
        
        // 处理子节点
        addIndent();
        for (auto& child : element->getChildren()) {
            generateNode(child);
        }
        removeIndent();
        
        // 结束标签
        htmlCode_ += getIndent() + "</" + tagName + ">\n";
    }
}

void CHTLGenerator::generateText(std::shared_ptr<TextNode> text) {
    std::string content = escapeHTML(text->getText());
    htmlCode_ += getIndent() + content + "\n";
}

void CHTLGenerator::generateStyle(std::shared_ptr<StyleNode> style) {
    if (inStyleBlock_) {
        // 局部样式块
        std::string properties = generateStyleProperties(style->getProperties());
        if (!properties.empty()) {
            cssCode_ += getIndent() + properties + "\n";
        }
    } else {
        // 全局样式块
        cssCode_ += getIndent() + "<style>\n";
        addIndent();
        
        std::string properties = generateStyleProperties(style->getProperties());
        if (!properties.empty()) {
            cssCode_ += getIndent() + properties + "\n";
        }
        
        removeIndent();
        cssCode_ += getIndent() + "</style>\n";
    }
}

void CHTLGenerator::generateScript(std::shared_ptr<ScriptNode> script) {
    if (inScriptBlock_) {
        // 局部脚本块
        std::string scriptContent = escapeJS(script->getScript());
        if (!scriptContent.empty()) {
            jsCode_ += getIndent() + scriptContent + "\n";
        }
    } else {
        // 全局脚本块
        jsCode_ += getIndent() + "<script>\n";
        addIndent();
        
        std::string scriptContent = escapeJS(script->getScript());
        if (!scriptContent.empty()) {
            jsCode_ += getIndent() + scriptContent + "\n";
        }
        
        removeIndent();
        jsCode_ += getIndent() + "</script>\n";
    }
}

void CHTLGenerator::generateTemplate(std::shared_ptr<TemplateNode> template_) {
    // 模板处理逻辑
    std::string templateType = template_->getTemplateType();
    std::string templateName = template_->getTemplateName();
    
    // 将模板存储到上下文中
    context_.addTemplate(templateName, templateType, template_->toString());
    
    // 生成模板内容
    for (auto& child : template_->getChildren()) {
        generateNode(child);
    }
}

void CHTLGenerator::generateCustom(std::shared_ptr<CustomNode> custom) {
    // 自定义处理逻辑
    std::string customType = custom->getCustomType();
    std::string customName = custom->getCustomName();
    
    // 将自定义存储到上下文中
    context_.addCustom(customName, customType, custom->toString());
    
    // 生成自定义内容
    for (auto& child : custom->getChildren()) {
        generateNode(child);
    }
}

void CHTLGenerator::generateOrigin(std::shared_ptr<OriginNode> origin) {
    std::string originType = origin->getOriginType();
    std::string content = origin->getText();
    
    if (originType == "Html" || originType == "HTML") {
        htmlCode_ += content + "\n";
    } else if (originType == "Style" || originType == "CSS") {
        cssCode_ += content + "\n";
    } else if (originType == "JavaScript" || originType == "JS") {
        jsCode_ += content + "\n";
    }
}

void CHTLGenerator::generateImport(std::shared_ptr<ImportNode> import_) {
    std::string importType = import_->getImportType();
    std::string importPath = import_->getImportPath();
    std::string importAlias = import_->getImportAlias();
    
    // 处理导入逻辑
    if (importType == "Html" || importType == "HTML") {
        std::string content = readFile(importPath);
        htmlCode_ += content + "\n";
    } else if (importType == "Style" || importType == "CSS") {
        std::string content = readFile(importPath);
        cssCode_ += content + "\n";
    } else if (importType == "JavaScript" || importType == "JS") {
        std::string content = readFile(importPath);
        jsCode_ += content + "\n";
    } else if (importType == "Chtl" || importType == "CHTL") {
        // 处理CHTL文件导入
        // 这里需要递归处理导入的CHTL文件
    }
}

void CHTLGenerator::generateNamespace(std::shared_ptr<NamespaceNode> namespace_) {
    std::string namespaceName = namespace_->getName();
    
    // 将命名空间存储到上下文中
    context_.addNamespace(namespaceName, namespace_->toString());
    
    // 生成命名空间内容
    for (auto& child : namespace_->getChildren()) {
        generateNode(child);
    }
}

void CHTLGenerator::generateConfiguration(std::shared_ptr<ConfigurationNode> config) {
    // 配置处理逻辑
    for (const auto& item : config->getConfigItems()) {
        context_.setConfiguration(item.first, item.second);
    }
}

void CHTLGenerator::generateConstraint(std::shared_ptr<ConstraintNode> constraint) {
    // 约束处理逻辑
}

void CHTLGenerator::generateUse(std::shared_ptr<UseNode> use) {
    std::string useTarget = use->getUseTarget();
    
    // 处理use语句
    if (useTarget == "html5") {
        htmlCode_ = "<!DOCTYPE html>\n" + htmlCode_;
    } else {
        // 处理其他use目标
    }
}

// AST访问者接口实现
void CHTLGenerator::visitElement(ElementNode& node) {
    generateElement(std::make_shared<ElementNode>(node));
}

void CHTLGenerator::visitText(TextNode& node) {
    generateText(std::make_shared<TextNode>(node));
}

void CHTLGenerator::visitStyle(StyleNode& node) {
    generateStyle(std::make_shared<StyleNode>(node));
}

void CHTLGenerator::visitScript(ScriptNode& node) {
    generateScript(std::make_shared<ScriptNode>(node));
}

void CHTLGenerator::visitTemplate(TemplateNode& node) {
    generateTemplate(std::make_shared<TemplateNode>(node));
}

void CHTLGenerator::visitCustom(CustomNode& node) {
    generateCustom(std::make_shared<CustomNode>(node));
}

void CHTLGenerator::visitOrigin(OriginNode& node) {
    generateOrigin(std::make_shared<OriginNode>(node));
}

void CHTLGenerator::visitImport(ImportNode& node) {
    generateImport(std::make_shared<ImportNode>(node));
}

void CHTLGenerator::visitNamespace(NamespaceNode& node) {
    generateNamespace(std::make_shared<NamespaceNode>(node));
}

void CHTLGenerator::visitConfiguration(ConfigurationNode& node) {
    generateConfiguration(std::make_shared<ConfigurationNode>(node));
}

void CHTLGenerator::visitConstraint(ConstraintNode& node) {
    generateConstraint(std::make_shared<ConstraintNode>(node));
}

void CHTLGenerator::visitUse(UseNode& node) {
    generateUse(std::make_shared<UseNode>(node));
}

std::string CHTLGenerator::getIndent() const {
    return std::string(indentLevel_ * 2, ' ');
}

void CHTLGenerator::addIndent() {
    indentLevel_++;
}

void CHTLGenerator::removeIndent() {
    if (indentLevel_ > 0) {
        indentLevel_--;
    }
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
    // CSS转义逻辑
    return text;
}

std::string CHTLGenerator::escapeJS(const std::string& text) {
    // JavaScript转义逻辑
    return text;
}

std::string CHTLGenerator::generateAttributes(const std::unordered_map<std::string, AttributeValue>& attributes) {
    std::string result;
    
    for (const auto& attr : attributes) {
        if (!result.empty()) {
            result += " ";
        }
        
        result += attr.first + "=\"";
        
        // 处理不同类型的属性值
        if (std::holds_alternative<std::string>(attr.second)) {
            result += escapeHTML(std::get<std::string>(attr.second));
        } else if (std::holds_alternative<double>(attr.second)) {
            result += std::to_string(std::get<double>(attr.second));
        } else if (std::holds_alternative<bool>(attr.second)) {
            result += std::get<bool>(attr.second) ? "true" : "false";
        }
        
        result += "\"";
    }
    
    return result;
}

std::string CHTLGenerator::generateStyleProperties(const std::unordered_map<std::string, std::string>& properties) {
    std::string result;
    
    for (const auto& prop : properties) {
        result += getIndent() + prop.first + ": " + prop.second + ";\n";
    }
    
    return result;
}

std::string CHTLGenerator::processTemplate(const std::string& template_, const std::string& name, const std::string& type) {
    // 模板处理逻辑
    return template_;
}

std::string CHTLGenerator::processCustom(const std::string& custom, const std::string& name, const std::string& type) {
    // 自定义处理逻辑
    return custom;
}

std::string CHTLGenerator::processExpression(const std::string& expression) {
    // 表达式处理逻辑
    return expression;
}

void CHTLGenerator::writeToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
    } else {
        reportError("无法写入文件: " + filename);
    }
}

std::string CHTLGenerator::readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    } else {
        reportError("无法读取文件: " + filename);
        return "";
    }
}

void CHTLGenerator::reportError(const std::string& message) {
    throw std::runtime_error("代码生成错误: " + message);
}

} // namespace CHTL