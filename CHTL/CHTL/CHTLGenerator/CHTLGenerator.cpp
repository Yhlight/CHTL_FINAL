#include "CHTLGenerator.h"
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cctype>

namespace CHTL {

CHTLGenerator::CHTLGenerator() 
    : prettyPrint(true), minify(false), includeComments(false), debugMode(false) {
    stats = {};
}

CHTLGenerator::CHTLGenerator(bool prettyPrint, bool minify, bool includeComments)
    : prettyPrint(prettyPrint), minify(minify), includeComments(includeComments), debugMode(false) {
    stats = {};
}

std::string CHTLGenerator::generate(std::shared_ptr<BaseNode> ast) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (!ast) {
        return "";
    }
    
    clearOutputs();
    
    try {
        // 生成HTML
        htmlOutput = generateHTML(ast);
        
        // 生成CSS
        cssOutput = generateCSS(ast);
        
        // 生成JS
        jsOutput = generateJS(ast);
        
        // 合并输出
        std::ostringstream oss;
        oss << "<!DOCTYPE html>\n";
        oss << "<html>\n";
        oss << "<head>\n";
        
        if (!cssOutput.empty()) {
            oss << "<style>\n" << cssOutput << "\n</style>\n";
        }
        
        oss << "</head>\n";
        oss << "<body>\n";
        oss << htmlOutput << "\n";
        oss << "</body>\n";
        
        if (!jsOutput.empty()) {
            oss << "<script>\n" << jsOutput << "\n</script>\n";
        }
        
        oss << "</html>\n";
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        stats.generateTime = duration.count();
        
        return oss.str();
        
    } catch (const std::exception& e) {
        if (debugMode) {
            std::cerr << "Generator error: " << e.what() << std::endl;
        }
        throw;
    }
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<BaseNode> ast) {
    std::string result;
    
    if (!ast) {
        return result;
    }
    
    // 遍历AST生成HTML
    ast->traverse([&](std::shared_ptr<BaseNode> node) {
        switch (node->getNodeType()) {
            case NodeType::ELEMENT:
                result += generateElement(node);
                break;
            case NodeType::TEXT:
                result += generateText(node);
                break;
            case NodeType::COMMENT:
                result += generateComment(node);
                break;
            case NodeType::TEMPLATE_STYLE:
            case NodeType::TEMPLATE_ELEMENT:
            case NodeType::TEMPLATE_VAR:
                processTemplate(node);
                break;
            case NodeType::CUSTOM_STYLE:
            case NodeType::CUSTOM_ELEMENT:
            case NodeType::CUSTOM_VAR:
                processCustom(node);
                break;
            case NodeType::STYLE:
                result += generateStyle(node);
                break;
            case NodeType::SCRIPT:
                result += generateScript(node);
                break;
            case NodeType::ORIGIN:
                result += generateOrigin(node);
                break;
            case NodeType::IMPORT:
                result += generateImport(node);
                break;
            case NodeType::NAMESPACE:
                result += generateNamespace(node);
                break;
            case NodeType::CONFIGURATION:
                result += generateConfiguration(node);
                break;
            default:
                break;
        }
    });
    
    return result;
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<BaseNode> ast) {
    std::string result;
    
    // 添加全局样式
    for (const auto& style : globalStyles) {
        result += style + "\n";
    }
    
    // 添加局部样式
    for (const auto& style : localStyles) {
        result += style + "\n";
    }
    
    return result;
}

std::string CHTLGenerator::generateJS(std::shared_ptr<BaseNode> ast) {
    std::string result;
    
    // 添加全局脚本
    for (const auto& script : globalScripts) {
        result += script + "\n";
    }
    
    // 添加局部脚本
    for (const auto& script : localScripts) {
        result += script + "\n";
    }
    
    return result;
}

std::string CHTLGenerator::generateElement(std::shared_ptr<BaseNode> node) {
    std::ostringstream oss;
    
    // 获取标签名
    std::string tagName = node->getName();
    if (tagName.empty()) {
        return "";
    }
    
    // 开始标签
    oss << "<" << tagName;
    
    // 属性
    std::string attributes = processAttributes(node);
    if (!attributes.empty()) {
        oss << " " << attributes;
    }
    
    // 自闭合标签
    if (isVoidElement(tagName)) {
        oss << " />";
        return oss.str();
    }
    
    oss << ">";
    
    // 子节点
    for (const auto& child : node->getChildren()) {
        switch (child->getNodeType()) {
            case NodeType::ELEMENT:
                oss << generateElement(child);
                break;
            case NodeType::TEXT:
                oss << generateText(child);
                break;
            case NodeType::COMMENT:
                oss << generateComment(child);
                break;
            default:
                break;
        }
    }
    
    // 结束标签
    oss << "</" << tagName << ">";
    
    return oss.str();
}

std::string CHTLGenerator::generateText(std::shared_ptr<BaseNode> node) {
    std::string content = node->getAttribute("content");
    if (content.empty()) {
        return "";
    }
    
    return escapeHTML(content);
}

std::string CHTLGenerator::generateComment(std::shared_ptr<BaseNode> node) {
    if (!includeComments) {
        return "";
    }
    
    std::string content = node->getAttribute("content");
    return "<!-- " + content + " -->";
}

std::string CHTLGenerator::generateStyle(std::shared_ptr<BaseNode> node) {
    std::string result = generateInlineStyle(node);
    if (!result.empty()) {
        localStyles.push_back(result);
    }
    return "";
}

std::string CHTLGenerator::generateScript(std::shared_ptr<BaseNode> node) {
    std::string result = generateInlineScript(node);
    if (!result.empty()) {
        localScripts.push_back(result);
    }
    return "";
}

std::string CHTLGenerator::generateTemplate(std::shared_ptr<BaseNode> node) {
    // 模板处理
    processTemplate(node);
    return "";
}

std::string CHTLGenerator::generateCustom(std::shared_ptr<BaseNode> node) {
    // 自定义处理
    processCustom(node);
    return "";
}

std::string CHTLGenerator::generateOrigin(std::shared_ptr<BaseNode> node) {
    std::string type = node->getAttribute("type");
    std::string content = node->getAttribute("content");
    
    if (type == "html") {
        return content;
    } else if (type == "css") {
        globalStyles.push_back(content);
        return "";
    } else if (type == "js") {
        globalScripts.push_back(content);
        return "";
    }
    
    return content;
}

std::string CHTLGenerator::generateImport(std::shared_ptr<BaseNode> node) {
    // 导入处理
    return "";
}

std::string CHTLGenerator::generateNamespace(std::shared_ptr<BaseNode> node) {
    // 命名空间处理
    return "";
}

std::string CHTLGenerator::generateConfiguration(std::shared_ptr<BaseNode> node) {
    // 配置处理
    return "";
}

std::string CHTLGenerator::generateInlineStyle(std::shared_ptr<BaseNode> node) {
    std::ostringstream oss;
    
    // 生成内联样式
    for (const auto& attr : node->getAttributes()) {
        if (attr.first != "content") {
            oss << attr.first << ": " << attr.second << "; ";
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateInlineScript(std::shared_ptr<BaseNode> node) {
    return node->getAttribute("content");
}

void CHTLGenerator::processTemplate(std::shared_ptr<BaseNode> node) {
    std::string name = node->getAttribute("name");
    if (!name.empty()) {
        registerTemplate(name, node);
    }
}

void CHTLGenerator::processCustom(std::shared_ptr<BaseNode> node) {
    std::string name = node->getAttribute("name");
    if (!name.empty()) {
        registerCustom(name, node);
    }
}

std::string CHTLGenerator::processAttributes(std::shared_ptr<BaseNode> node) {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& attr : node->getAttributes()) {
        if (!first) {
            oss << " ";
        }
        
        oss << attr.first;
        if (!attr.second.empty()) {
            oss << "=\"" << escapeHTML(attr.second) << "\"";
        }
        
        first = false;
    }
    
    return oss.str();
}

std::string CHTLGenerator::escapeHTML(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2);
    
    for (char c : str) {
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

std::string CHTLGenerator::escapeCSS(const std::string& str) {
    // CSS转义
    return str;
}

std::string CHTLGenerator::escapeJS(const std::string& str) {
    // JS转义
    return str;
}

std::string CHTLGenerator::indent(int level) {
    return std::string(level * 2, ' ');
}

bool CHTLGenerator::isVoidElement(const std::string& tagName) {
    static const std::vector<std::string> voidElements = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return std::find(voidElements.begin(), voidElements.end(), tagName) != voidElements.end();
}

void CHTLGenerator::pushContext(std::shared_ptr<BaseNode> node) {
    contextStack.push_back(node);
}

void CHTLGenerator::popContext() {
    if (!contextStack.empty()) {
        contextStack.pop_back();
    }
}

std::shared_ptr<BaseNode> CHTLGenerator::getCurrentContext() const {
    if (contextStack.empty()) {
        return nullptr;
    }
    return contextStack.back();
}

void CHTLGenerator::registerTemplate(const std::string& name, std::shared_ptr<BaseNode> template) {
    templates[name] = template;
}

std::shared_ptr<BaseNode> CHTLGenerator::getTemplate(const std::string& name) const {
    auto it = templates.find(name);
    if (it != templates.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLGenerator::clearTemplates() {
    templates.clear();
}

void CHTLGenerator::registerCustom(const std::string& name, std::shared_ptr<BaseNode> custom) {
    customs[name] = custom;
}

std::shared_ptr<BaseNode> CHTLGenerator::getCustom(const std::string& name) const {
    auto it = customs.find(name);
    if (it != customs.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLGenerator::clearCustoms() {
    customs.clear();
}

void CHTLGenerator::setVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

std::string CHTLGenerator::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    return "";
}

void CHTLGenerator::clearVariables() {
    variables.clear();
}

void CHTLGenerator::reset() {
    clearOutputs();
    clearTemplates();
    clearCustoms();
    clearVariables();
    contextStack.clear();
    globalStyles.clear();
    localStyles.clear();
    globalScripts.clear();
    localScripts.clear();
    resetStats();
}

void CHTLGenerator::clearOutputs() {
    htmlOutput.clear();
    cssOutput.clear();
    jsOutput.clear();
}

void CHTLGenerator::enableDebugMode(bool enable) {
    debugMode = enable;
}

std::string CHTLGenerator::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTLGenerator Debug Info:\n";
    oss << "Templates: " << templates.size() << "\n";
    oss << "Customs: " << customs.size() << "\n";
    oss << "Variables: " << variables.size() << "\n";
    oss << "Global Styles: " << globalStyles.size() << "\n";
    oss << "Local Styles: " << localStyles.size() << "\n";
    return oss.str();
}

CHTLGenerator::GenerateStats CHTLGenerator::getStats() const {
    return stats;
}

void CHTLGenerator::resetStats() {
    stats = {};
}

} // namespace CHTL