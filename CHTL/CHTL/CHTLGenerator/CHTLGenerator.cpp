#include "CHTLGenerator.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLGenerator::CHTLGenerator() {
    // 默认配置
    configuration_["HTML5_DOCTYPE"] = "true";
    configuration_["MINIFY_OUTPUT"] = "false";
    configuration_["INLINE_CSS"] = "false";
    configuration_["INLINE_JS"] = "false";
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<BaseNode> root) {
    std::ostringstream oss;
    
    // HTML5文档类型
    if (configuration_["HTML5_DOCTYPE"] == "true") {
        oss << "<!DOCTYPE html>\n";
    }
    
    // 处理模板和自定义
    processTemplates(root);
    processCustoms(root);
    
    // 生成HTML内容
    for (const auto& child : root->getChildren()) {
        std::string html = child->toHTML();
        if (!html.empty()) {
            oss << html << "\n";
        }
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<BaseNode> root) {
    global_css_.clear();
    
    // 处理样式
    processStyles(root);
    
    std::ostringstream oss;
    for (const auto& css : global_css_) {
        oss << css << "\n";
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateJS(std::shared_ptr<BaseNode> root) {
    global_js_.clear();
    
    // 处理脚本
    processScripts(root);
    
    std::ostringstream oss;
    for (const auto& js : global_js_) {
        oss << js << "\n";
    }
    
    return oss.str();
}

std::string CHTLGenerator::generateDocument(std::shared_ptr<BaseNode> root) {
    std::ostringstream oss;
    
    // HTML5文档类型
    if (configuration_["HTML5_DOCTYPE"] == "true") {
        oss << "<!DOCTYPE html>\n";
    }
    
    oss << "<html>\n";
    oss << "<head>\n";
    
    // 添加CSS
    std::string css = generateCSS(root);
    if (!css.empty()) {
        if (configuration_["INLINE_CSS"] == "true") {
            oss << "<style>\n" << css << "</style>\n";
        } else {
            oss << "<link rel=\"stylesheet\" href=\"style.css\">\n";
        }
    }
    
    oss << "</head>\n";
    oss << "<body>\n";
    
    // 添加HTML内容
    for (const auto& child : root->getChildren()) {
        std::string html = child->toHTML();
        if (!html.empty()) {
            oss << html << "\n";
        }
    }
    
    oss << "</body>\n";
    
    // 添加JavaScript
    std::string js = generateJS(root);
    if (!js.empty()) {
        if (configuration_["INLINE_JS"] == "true") {
            oss << "<script>\n" << js << "</script>\n";
        } else {
            oss << "<script src=\"script.js\"></script>\n";
        }
    }
    
    oss << "</html>\n";
    
    return oss.str();
}

void CHTLGenerator::setConfiguration(const std::unordered_map<std::string, std::string>& config) {
    configuration_ = config;
}

std::string CHTLGenerator::generateElementHTML(std::shared_ptr<BaseNode> node) {
    std::ostringstream oss;
    
    // 开始标签
    oss << "<" << node->getName();
    
    // 属性
    for (const auto& attr : node->getAttributes()) {
        oss << " " << attr.first;
        if (!attr.second.empty()) {
            oss << "=\"" << escapeHTML(attr.second) << "\"";
        }
    }
    
    oss << ">";
    
    // 子节点
    for (const auto& child : node->getChildren()) {
        oss << child->toHTML();
    }
    
    // 结束标签
    oss << "</" << node->getName() << ">";
    
    return oss.str();
}

std::string CHTLGenerator::generateTextHTML(std::shared_ptr<BaseNode> node) {
    return escapeHTML(node->getName());
}

std::string CHTLGenerator::generateTemplateHTML(std::shared_ptr<BaseNode> node) {
    // 模板节点不直接生成HTML，但会处理其子节点
    std::ostringstream oss;
    for (const auto& child : node->getChildren()) {
        oss << child->toHTML();
    }
    return oss.str();
}

std::string CHTLGenerator::generateCustomHTML(std::shared_ptr<BaseNode> node) {
    // 自定义节点不直接生成HTML，但会处理其子节点
    std::ostringstream oss;
    for (const auto& child : node->getChildren()) {
        oss << child->toHTML();
    }
    return oss.str();
}

std::string CHTLGenerator::generateOriginHTML(std::shared_ptr<BaseNode> node) {
    std::string type = node->getName();
    std::string content = node->getAttribute("content");
    
    if (type == "@Html") {
        return content;
    } else if (type == "@Style") {
        global_css_.push_back(content);
        return "";
    } else if (type == "@JavaScript") {
        global_js_.push_back(content);
        return "";
    }
    
    return "";
}

std::string CHTLGenerator::generateElementCSS(std::shared_ptr<BaseNode> node) {
    // 元素节点不直接生成CSS
    return "";
}

std::string CHTLGenerator::generateStyleCSS(std::shared_ptr<BaseNode> node) {
    std::string content = node->getAttribute("content");
    if (!content.empty()) {
        global_css_.push_back(content);
    }
    return "";
}

std::string CHTLGenerator::generateTemplateCSS(std::shared_ptr<BaseNode> node) {
    // 模板节点不直接生成CSS
    return "";
}

std::string CHTLGenerator::generateCustomCSS(std::shared_ptr<BaseNode> node) {
    // 自定义节点不直接生成CSS
    return "";
}

std::string CHTLGenerator::generateElementJS(std::shared_ptr<BaseNode> node) {
    // 元素节点不直接生成JS
    return "";
}

std::string CHTLGenerator::generateScriptJS(std::shared_ptr<BaseNode> node) {
    std::string content = node->getAttribute("content");
    if (!content.empty()) {
        global_js_.push_back(content);
    }
    return "";
}

std::string CHTLGenerator::generateTemplateJS(std::shared_ptr<BaseNode> node) {
    // 模板节点不直接生成JS
    return "";
}

std::string CHTLGenerator::generateCustomJS(std::shared_ptr<BaseNode> node) {
    // 自定义节点不直接生成JS
    return "";
}

std::string CHTLGenerator::escapeHTML(const std::string& text) {
    std::ostringstream oss;
    for (char c : text) {
        switch (c) {
            case '<': oss << "&lt;"; break;
            case '>': oss << "&gt;"; break;
            case '&': oss << "&amp;"; break;
            case '"': oss << "&quot;"; break;
            case '\'': oss << "&#39;"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string CHTLGenerator::escapeCSS(const std::string& text) {
    // CSS转义逻辑
    return text;
}

std::string CHTLGenerator::escapeJS(const std::string& text) {
    // JavaScript转义逻辑
    return text;
}

void CHTLGenerator::processTemplates(std::shared_ptr<BaseNode> root) {
    // 处理模板逻辑
    for (const auto& child : root->getChildren()) {
        if (child->getType() == NodeType::TEMPLATE_STYLE ||
            child->getType() == NodeType::TEMPLATE_ELEMENT ||
            child->getType() == NodeType::TEMPLATE_VAR) {
            // 模板处理逻辑
        }
    }
}

void CHTLGenerator::processCustoms(std::shared_ptr<BaseNode> root) {
    // 处理自定义逻辑
    for (const auto& child : root->getChildren()) {
        if (child->getType() == NodeType::CUSTOM_STYLE ||
            child->getType() == NodeType::CUSTOM_ELEMENT ||
            child->getType() == NodeType::CUSTOM_VAR) {
            // 自定义处理逻辑
        }
    }
}

void CHTLGenerator::processStyles(std::shared_ptr<BaseNode> node) {
    // 处理样式
    if (node->getType() == NodeType::STYLE) {
        generateStyleCSS(node);
    }
    
    for (const auto& child : node->getChildren()) {
        processStyles(child);
    }
}

void CHTLGenerator::processStyleBlock(std::shared_ptr<BaseNode> node) {
    // 处理样式块
    auto styleNodes = node->findChildren(NodeType::STYLE);
    for (const auto& styleNode : styleNodes) {
        processStyleBlock(styleNode);
    }
}

void CHTLGenerator::processScripts(std::shared_ptr<BaseNode> node) {
    // 处理脚本
    if (node->getType() == NodeType::SCRIPT) {
        generateScriptJS(node);
    }
    
    for (const auto& child : node->getChildren()) {
        processScripts(child);
    }
}

void CHTLGenerator::processScriptBlock(std::shared_ptr<BaseNode> node) {
    // 处理脚本块
    auto scriptNodes = node->findChildren(NodeType::SCRIPT);
    for (const auto& scriptNode : scriptNodes) {
        processScriptBlock(scriptNode);
    }
}

} // namespace CHTL