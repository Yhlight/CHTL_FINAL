#include "CodeGenerator.hpp"
#include <CHTL/CHTLNode/ElementNode.hpp>
#include <CHTL/CHTLNode/TextNode.hpp>
#include <CHTL/CHTLNode/StyleNode.hpp>
#include <CHTL/CHTLNode/TemplateStyleNode.hpp>
#include <CHTL/CHTLNode/TemplateElementNode.hpp>
#include <CHTL/CHTLNode/TemplateVarNode.hpp>
#include <CHTL/CHTLNode/CustomStyleNode.hpp>
#include <CHTL/CHTLNode/CustomElementNode.hpp>
#include <CHTL/CHTLNode/CustomVarNode.hpp>
#include <CHTL/CHTLNode/OriginNode.hpp>
#include <CHTL/CHTLNode/ImportNode.hpp>
#include <CHTL/CHTLNode/ConfigNode.hpp>
#include <CHTL/CHTLNode/NamespaceNode.hpp>
#include <CHTL/CHTLNode/ConstraintNode.hpp>
#include <sstream>
#include <algorithm>

namespace CHTL {

CodeGenerator::CodeGenerator() : templateManager_(TemplateManager::getInstance()) {}

std::string CodeGenerator::generateHTML(std::shared_ptr<BaseNode> root) {
    if (!root) return "";
    
    std::ostringstream html;
    
    // 生成 HTML 文档结构
    html << "<!DOCTYPE html>\n<html>\n<head>\n";
    
    // 生成 CSS
    std::string css = generateCSS(root);
    if (!css.empty()) {
        html << "<style>\n" << css << "\n</style>\n";
    }
    
    html << "</head>\n<body>\n";
    
    // 生成主体内容
    for (const auto& child : root->getChildren()) {
        if (child) {
            html << generateElementHTML(child);
        }
    }
    
    html << "</body>\n</html>\n";
    
    return html.str();
}

std::string CodeGenerator::generateCSS(std::shared_ptr<BaseNode> root) {
    if (!root) return "";
    
    std::ostringstream css;
    
    // 遍历所有节点生成 CSS
    generateStyleCSS(root);
    
    // 输出生成的 CSS
    for (const auto& [selector, properties] : generatedCSS_) {
        css << selector << " {\n";
        css << properties;
        css << "}\n\n";
    }
    
    return css.str();
}

std::string CodeGenerator::generateJavaScript(std::shared_ptr<BaseNode> root) {
    if (!root) return "";
    
    std::ostringstream js;
    
    // 遍历所有节点生成 JavaScript
    for (const auto& child : root->getChildren()) {
        if (child) {
            std::string childJS = generateOriginCode(child);
            if (!childJS.empty()) {
                js << childJS << "\n";
            }
        }
    }
    
    return js.str();
}

std::string CodeGenerator::generateOutput(std::shared_ptr<BaseNode> root) {
    std::string html = generateHTML(root);
    std::string css = generateCSS(root);
    std::string js = generateJavaScript(root);
    
    return formatOutput(html, css, js);
}

std::string CodeGenerator::generateElementHTML(std::shared_ptr<BaseNode> node) {
    if (!node) return "";
    
    switch (node->getType()) {
        case BaseNode::NodeType::ELEMENT: {
            auto elementNode = std::dynamic_pointer_cast<ElementNode>(node);
            if (!elementNode) return "";
            
            std::ostringstream html;
            html << "<" << elementNode->getTagName();
            
            // 生成属性
            std::string attributes = generateAttributes(elementNode->getAttributes());
            if (!attributes.empty()) {
                html << " " << attributes;
            }
            
            html << ">";
            
            // 生成子节点
            for (const auto& child : elementNode->getChildren()) {
                if (child) {
                    html << generateElementHTML(child);
                }
            }
            
            html << "</" << elementNode->getTagName() << ">";
            return html.str();
        }
        
        case BaseNode::NodeType::TEXT: {
            auto textNode = std::dynamic_pointer_cast<TextNode>(node);
            if (!textNode) return "";
            return textNode->getValue();
        }
        
        case BaseNode::NodeType::STYLE: {
            auto styleNode = std::dynamic_pointer_cast<StyleNode>(node);
            if (!styleNode) return "";
            
            // 生成内联样式
            std::string style = generateCSSProperties(styleNode->getCSSProperties());
            if (!style.empty()) {
                return " style=\"" + style + "\"";
            }
            return "";
        }
        
        case BaseNode::NodeType::TEMPLATE: {
            return generateTemplateCode(node);
        }
        
        case BaseNode::NodeType::CUSTOM: {
            return generateCustomCode(node);
        }
        
        case BaseNode::NodeType::ORIGIN: {
            return generateOriginCode(node);
        }
        
        case BaseNode::NodeType::IMPORT: {
            return generateImportCode(node);
        }
        
        case BaseNode::NodeType::CONFIG: {
            return generateConfigCode(node);
        }
        
        case BaseNode::NodeType::NAMESPACE: {
            return generateNamespaceCode(node);
        }
        
        case BaseNode::NodeType::OPERATOR: {
            return generateConstraintCode(node);
        }
        
        default:
            return "";
    }
}

std::string CodeGenerator::generateTextHTML(std::shared_ptr<BaseNode> node) {
    auto textNode = std::dynamic_pointer_cast<TextNode>(node);
    if (!textNode) return "";
    
    return textNode->getValue();
}

std::string CodeGenerator::generateStyleCSS(std::shared_ptr<BaseNode> node) {
    if (!node) return "";
    
    std::string css;
    
    // 处理样式节点
    if (node->getType() == BaseNode::NodeType::STYLE) {
        auto styleNode = std::dynamic_pointer_cast<StyleNode>(node);
        if (styleNode) {
            std::string selector = "." + styleNode->getAttribute("class");
            if (selector == ".") selector = "body";
            
            std::string properties = generateCSSProperties(styleNode->getCSSProperties());
            if (!properties.empty()) {
                generatedCSS_[selector] = properties;
            }
        }
    }
    
    // 递归处理子节点
    for (const auto& child : node->getChildren()) {
        if (child) {
            generateStyleCSS(child);
        }
    }
    
    return css;
}

std::string CodeGenerator::generateTemplateCode(std::shared_ptr<BaseNode> node) {
    // 模板节点不直接生成 HTML，而是被其他节点引用
    return "";
}

std::string CodeGenerator::generateCustomCode(std::shared_ptr<BaseNode> node) {
    // 自定义节点不直接生成 HTML，而是被其他节点引用
    return "";
}

std::string CodeGenerator::generateOriginCode(std::shared_ptr<BaseNode> node) {
    auto originNode = std::dynamic_pointer_cast<OriginNode>(node);
    if (!originNode) return "";
    
    std::string type = originNode->getAttribute("type");
    if (type == "@Html") {
        return originNode->getValue();
    } else if (type == "@Style") {
        // 将原始 CSS 添加到生成的 CSS 中
        std::string name = originNode->getAttribute("name");
        if (!name.empty()) {
            generatedCSS_[name] = originNode->getValue();
        }
        return "";
    } else if (type == "@JavaScript") {
        // 将原始 JavaScript 添加到生成的 JS 中
        std::string name = originNode->getAttribute("name");
        if (!name.empty()) {
            generatedJS_[name] = originNode->getValue();
        }
        return "";
    }
    
    return "";
}

std::string CodeGenerator::generateImportCode(std::shared_ptr<BaseNode> node) {
    auto importNode = std::dynamic_pointer_cast<ImportNode>(node);
    if (!importNode) return "";
    
    // 导入节点不直接生成 HTML，而是被其他节点引用
    return "";
}

std::string CodeGenerator::generateConfigCode(std::shared_ptr<BaseNode> node) {
    auto configNode = std::dynamic_pointer_cast<ConfigNode>(node);
    if (!configNode) return "";
    
    // 配置节点不直接生成 HTML，而是被其他节点引用
    return "";
}

std::string CodeGenerator::generateNamespaceCode(std::shared_ptr<BaseNode> node) {
    auto namespaceNode = std::dynamic_pointer_cast<NamespaceNode>(node);
    if (!namespaceNode) return "";
    
    // 命名空间节点不直接生成 HTML，而是被其他节点引用
    return "";
}

std::string CodeGenerator::generateConstraintCode(std::shared_ptr<BaseNode> node) {
    auto constraintNode = std::dynamic_pointer_cast<ConstraintNode>(node);
    if (!constraintNode) return "";
    
    // 约束节点不直接生成 HTML，而是被其他节点引用
    return "";
}

std::string CodeGenerator::resolveTemplateReference(const std::string& reference) {
    // 解析模板引用
    auto styleTemplate = templateManager_.getStyleTemplate(reference);
    if (styleTemplate) {
        return styleTemplate->toCSS();
    }
    
    auto elementTemplate = templateManager_.getElementTemplate(reference);
    if (elementTemplate) {
        return elementTemplate->toHTML();
    }
    
    auto varTemplate = templateManager_.getVarTemplate(reference);
    if (varTemplate) {
        return varTemplate->toHTML();
    }
    
    return reference;
}

std::string CodeGenerator::resolveVariableReference(const std::string& reference) {
    // 解析变量引用
    auto varTemplate = templateManager_.getVarTemplate(reference);
    if (varTemplate) {
        return varTemplate->getVariable(reference);
    }
    
    return reference;
}

std::string CodeGenerator::resolveStyleReference(const std::string& reference) {
    // 解析样式引用
    auto styleTemplate = templateManager_.getStyleTemplate(reference);
    if (styleTemplate) {
        return styleTemplate->toCSS();
    }
    
    return reference;
}

std::string CodeGenerator::generateAttributes(const std::map<std::string, std::string>& attributes) {
    std::ostringstream attrs;
    
    for (const auto& [key, value] : attributes) {
        if (!value.empty()) {
            attrs << key << "=\"" << value << "\" ";
        } else {
            attrs << key << " ";
        }
    }
    
    std::string result = attrs.str();
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

std::string CodeGenerator::generateCSSProperties(const std::map<std::string, std::string>& properties) {
    std::ostringstream css;
    
    for (const auto& [property, value] : properties) {
        css << property << ": " << value << "; ";
    }
    
    std::string result = css.str();
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

std::string CodeGenerator::formatOutput(const std::string& html, const std::string& css, const std::string& js) {
    std::ostringstream output;
    
    output << "=== 生成的 HTML ===\n";
    output << html << "\n\n";
    
    if (!css.empty()) {
        output << "=== 生成的 CSS ===\n";
        output << css << "\n\n";
    }
    
    if (!js.empty()) {
        output << "=== 生成的 JavaScript ===\n";
        output << js << "\n\n";
    }
    
    return output.str();
}

} // namespace CHTL