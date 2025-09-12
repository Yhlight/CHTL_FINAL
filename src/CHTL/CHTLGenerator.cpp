#include "CHTLGenerator.h"
#include <sstream>

namespace CHTL {

CHTLGenerator::CHTLGenerator() {}

CHTLGenerator::~CHTLGenerator() {}

std::string CHTLGenerator::generateHTML(const ASTNode* ast) {
    std::stringstream html;
    
    for (const auto& child : ast->children) {
        if (child->getType() == NodeType::ELEMENT) {
            html << generateElement(static_cast<const ElementNode*>(child.get()));
        }
    }
    
    return html.str();
}

std::string CHTLGenerator::generateCSS(const ASTNode* ast) {
    return globalCSS;
}

std::string CHTLGenerator::generateJavaScript(const ASTNode* ast) {
    return globalJS;
}

std::string CHTLGenerator::generateCompleteHTML(const ASTNode* ast) {
    std::stringstream html;
    
    html << "<!DOCTYPE html>\n<html>\n<head>\n";
    
    if (!globalCSS.empty()) {
        html << "<style>\n" << globalCSS << "\n</style>\n";
    }
    
    html << "</head>\n<body>\n";
    html << generateHTML(ast);
    html << "</body>\n";
    
    if (!globalJS.empty()) {
        html << "<script>\n" << globalJS << "\n</script>\n";
    }
    
    html << "</html>";
    
    return html.str();
}

std::string CHTLGenerator::generateElement(const ElementNode* element) {
    std::stringstream html;
    
    html << "<" << element->getTagName();
    
    // 生成属性
    for (const auto& attr : element->getAttributes()) {
        html << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    html << ">";
    
    // 生成子元素
    for (const auto& child : element->getChildren()) {
        if (child->getType() == NodeType::ELEMENT) {
            html << generateElement(static_cast<const ElementNode*>(child.get()));
        } else if (child->getType() == NodeType::TEXT) {
            html << static_cast<const TextNode*>(child.get())->getText();
        }
    }
    
    html << "</" << element->getTagName() << ">";
    
    return html.str();
}

std::string CHTLGenerator::generateStyle(const StyleNode* style) {
    std::stringstream css;
    
    for (const auto& prop : style->getProperties()) {
        css << prop.first << ": " << prop.second << ";\n";
    }
    
    return css.str();
}

std::string CHTLGenerator::generateScript(const ScriptNode* script) {
    return script->getScript();
}

std::string CHTLGenerator::generateTemplate(const TemplateNode* templateNode) {
    // 模板处理逻辑
    return "";
}

std::string CHTLGenerator::generateCustom(const CustomNode* customNode) {
    // 自定义处理逻辑
    return "";
}

std::string CHTLGenerator::generateImport(const ImportNode* importNode) {
    // 导入处理逻辑
    return "";
}

std::string CHTLGenerator::generateNamespace(const NamespaceNode* namespaceNode) {
    // 命名空间处理逻辑
    return "";
}

std::string CHTLGenerator::generateOrigin(const OriginNode* origin) {
    return origin->getOriginContent();
}

std::string CHTLGenerator::generateConfiguration(const ConfigurationNode* config) {
    // 配置处理逻辑
    return "";
}

} // namespace CHTL