#include "CHTL/CHTLGenerator.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLGenerator::CHTLGenerator() 
    : default_structure_(false), indent_char_("  "), minify_(false) {
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<CHTLNode> ast) {
    if (!ast) return "";
    
    generated_css_.clear();
    generated_js_.clear();
    
    std::stringstream html;
    
    if (default_structure_) {
        html << "<!DOCTYPE html>" << (minify_ ? "" : "\n");
        html << "<html>" << (minify_ ? "" : "\n");
        html << "<head>" << (minify_ ? "" : "\n");
        html << getIndent(1) << "<meta charset=\"UTF-8\">" << (minify_ ? "" : "\n");
        
        if (!generated_css_.empty()) {
            html << getIndent(1) << "<style>" << (minify_ ? "" : "\n");
            html << generated_css_;
            html << (minify_ ? "" : "\n") << getIndent(1) << "</style>" << (minify_ ? "" : "\n");
        }
        
        html << "</head>" << (minify_ ? "" : "\n");
        html << "<body>" << (minify_ ? "" : "\n");
    }
    
    // 生成根节点的子节点
    for (const auto& child : ast->getChildren()) {
        html << generateElementHTML(child, default_structure_ ? 1 : 0);
    }
    
    if (default_structure_) {
        if (!generated_js_.empty()) {
            html << getIndent(1) << "<script>" << (minify_ ? "" : "\n");
            html << generated_js_;
            html << (minify_ ? "" : "\n") << getIndent(1) << "</script>" << (minify_ ? "" : "\n");
        }
        
        html << "</body>" << (minify_ ? "" : "\n");
        html << "</html>" << (minify_ ? "" : "\n");
    }
    
    return html.str();
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<CHTLNode> ast) {
    if (!ast) return "";
    
    generated_css_.clear();
    
    // 遍历AST收集所有样式
    // collectStyles(ast);
    
    return generated_css_;
}

std::string CHTLGenerator::generateJavaScript(std::shared_ptr<CHTLNode> ast) {
    if (!ast) return "";
    
    generated_js_.clear();
    
    // 遍历AST收集所有脚本
    // collectScripts(ast);
    
    return generated_js_;
}

std::string CHTLGenerator::generateCompleteHTML(std::shared_ptr<CHTLNode> ast) {
    return generateHTML(ast);
}

void CHTLGenerator::setDefaultStructure(bool enabled) {
    default_structure_ = enabled;
}

void CHTLGenerator::setIndent(const std::string& indent) {
    indent_char_ = indent;
}

void CHTLGenerator::setMinify(bool minify) {
    minify_ = minify;
}

std::string CHTLGenerator::generateElementHTML(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream html;
    std::string indent = getIndent(indent_level);
    
    switch (node->getType()) {
        case NodeType::ELEMENT: {
            std::string tag_name = node->getName();
            std::string attributes = generateAttributes(node);
            
            html << indent << "<" << tag_name;
            if (!attributes.empty()) {
                html << " " << attributes;
            }
            
            if (node->getChildren().empty() && node->getTextContent().empty()) {
                // 自闭合标签
                html << " />" << (minify_ ? "" : "\n");
            } else {
                html << ">" << (minify_ ? "" : "\n");
                
                // 生成文本内容
                if (!node->getTextContent().empty()) {
                    html << getIndent(indent_level + 1) << escapeHTML(node->getTextContent()) << (minify_ ? "" : "\n");
                }
                
                // 生成子节点
                for (const auto& child : node->getChildren()) {
                    html << generateElementHTML(child, indent_level + 1);
                }
                
                html << indent << "</" << tag_name << ">" << (minify_ ? "" : "\n");
            }
            break;
        }
        
        case NodeType::TEXT: {
            html << indent << escapeHTML(node->getTextContent()) << (minify_ ? "" : "\n");
            break;
        }
        
        case NodeType::COMMENT: {
            html << indent << "<!-- " << node->getTextContent() << " -->" << (minify_ ? "" : "\n");
            break;
        }
        
        case NodeType::STYLE: {
            html << generateStyleHTML(node, indent_level);
            break;
        }
        
        case NodeType::SCRIPT: {
            html << generateScriptHTML(node, indent_level);
            break;
        }
        
        case NodeType::TEMPLATE: {
            html << generateTemplateHTML(node, indent_level);
            break;
        }
        
        case NodeType::CUSTOM: {
            html << generateCustomHTML(node, indent_level);
            break;
        }
        
        case NodeType::ORIGIN: {
            html << generateOriginHTML(node, indent_level);
            break;
        }
        
        default:
            // 递归处理子节点
            for (const auto& child : node->getChildren()) {
                html << generateElementHTML(child, indent_level);
            }
            break;
    }
    
    return html.str();
}

std::string CHTLGenerator::generateTextHTML(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node || node->getType() != NodeType::TEXT) return "";
    
    std::string indent = getIndent(indent_level);
    return indent + escapeHTML(node->getTextContent()) + (minify_ ? "" : "\n");
}

std::string CHTLGenerator::generateCommentHTML(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node || node->getType() != NodeType::COMMENT) return "";
    
    std::string indent = getIndent(indent_level);
    return indent + "<!-- " + node->getTextContent() + " -->" + (minify_ ? "" : "\n");
}

std::string CHTLGenerator::generateStyleHTML(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node || node->getType() != NodeType::STYLE) return "";
    
    std::stringstream html;
    std::string indent = getIndent(indent_level);
    
    html << indent << "<style>" << (minify_ ? "" : "\n");
    
    // 生成CSS样式
    std::string css = generateCSSStyle(node, indent_level + 1);
    html << css;
    
    html << indent << "</style>" << (minify_ ? "" : "\n");
    
    return html.str();
}

std::string CHTLGenerator::generateScriptHTML(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node || node->getType() != NodeType::SCRIPT) return "";
    
    std::stringstream html;
    std::string indent = getIndent(indent_level);
    
    html << indent << "<script>" << (minify_ ? "" : "\n");
    
    // 生成JavaScript代码
    std::string js = generateJavaScriptCode(node, indent_level + 1);
    html << js;
    
    html << indent << "</script>" << (minify_ ? "" : "\n");
    
    return html.str();
}

std::string CHTLGenerator::generateTemplateHTML(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node || node->getType() != NodeType::TEMPLATE) return "";
    
    // 模板在生成时会被展开，这里暂时返回空
    // 实际实现中需要根据模板类型进行不同的处理
    return "";
}

std::string CHTLGenerator::generateCustomHTML(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node || node->getType() != NodeType::CUSTOM) return "";
    
    // 自定义元素在生成时会被展开，这里暂时返回空
    // 实际实现中需要根据自定义类型进行不同的处理
    return "";
}

std::string CHTLGenerator::generateOriginHTML(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node || node->getType() != NodeType::ORIGIN) return "";
    
    std::string indent = getIndent(indent_level);
    return indent + node->getTextContent() + (minify_ ? "" : "\n");
}

std::string CHTLGenerator::generateAttributes(std::shared_ptr<CHTLNode> node) {
    if (!node) return "";
    
    std::stringstream attrs;
    bool first = true;
    
    for (const auto& pair : node->getAttributes()) {
        if (!first) {
            attrs << " ";
        }
        first = false;
        
        attrs << pair.first << "=\"";
        
        // 处理属性值
        std::string value;
        if (std::holds_alternative<std::string>(pair.second)) {
            value = std::get<std::string>(pair.second);
        } else if (std::holds_alternative<int>(pair.second)) {
            value = std::to_string(std::get<int>(pair.second));
        } else if (std::holds_alternative<double>(pair.second)) {
            value = std::to_string(std::get<double>(pair.second));
        } else if (std::holds_alternative<bool>(pair.second)) {
            value = std::get<bool>(pair.second) ? "true" : "false";
        }
        
        // 转义属性值
        value = escapeHTML(value);
        attrs << value << "\"";
    }
    
    return attrs.str();
}

std::string CHTLGenerator::generateCSSStyle(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::stringstream css;
    std::string indent = getIndent(indent_level);
    
    // 处理局部样式块
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::ELEMENT) {
            std::string selector = child->getName();
            
            // 处理类选择器
            if (selector[0] == '.') {
                css << indent << selector << " {" << (minify_ ? "" : "\n");
                
                // 生成CSS属性
                for (const auto& pair : child->getAttributes()) {
                    std::string value;
                    if (std::holds_alternative<std::string>(pair.second)) {
                        value = std::get<std::string>(pair.second);
                    } else if (std::holds_alternative<int>(pair.second)) {
                        value = std::to_string(std::get<int>(pair.second));
                    } else if (std::holds_alternative<double>(pair.second)) {
                        value = std::to_string(std::get<double>(pair.second));
                    }
                    
                    css << getIndent(indent_level + 1) << pair.first << ": " << value << ";" << (minify_ ? "" : "\n");
                }
                
                css << indent << "}" << (minify_ ? "" : "\n");
            }
        }
    }
    
    return css.str();
}

std::string CHTLGenerator::generateJavaScriptCode(std::shared_ptr<CHTLNode> node, int indent_level) {
    if (!node) return "";
    
    std::string indent = getIndent(indent_level);
    std::string content = node->getTextContent();
    
    // 处理模板变量和自定义变量
    content = processTemplateVariables(content);
    content = processCustomVariables(content);
    
    return indent + content + (minify_ ? "" : "\n");
}

std::string CHTLGenerator::getIndent(int level) const {
    if (minify_) return "";
    
    std::string result;
    for (int i = 0; i < level; i++) {
        result += indent_char_;
    }
    return result;
}

std::string CHTLGenerator::escapeHTML(const std::string& text) const {
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

std::string CHTLGenerator::escapeCSS(const std::string& text) const {
    // CSS转义逻辑
    return text;
}

std::string CHTLGenerator::escapeJavaScript(const std::string& text) const {
    // JavaScript转义逻辑
    return text;
}

std::string CHTLGenerator::processTemplateVariables(const std::string& content) {
    // 处理模板变量替换
    std::string result = content;
    
    for (const auto& pair : template_variables_) {
        std::string placeholder = "{{" + pair.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), pair.second);
            pos += pair.second.length();
        }
    }
    
    return result;
}

std::string CHTLGenerator::processCustomVariables(const std::string& content) {
    // 处理自定义变量替换
    std::string result = content;
    
    for (const auto& pair : custom_variables_) {
        std::string placeholder = "{{" + pair.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), pair.second);
            pos += pair.second.length();
        }
    }
    
    return result;
}

std::string CHTLGenerator::processAttributeConditions(const std::string& content) {
    // 处理属性条件表达式
    // 这里需要实现复杂的条件表达式解析
    return content;
}

std::string CHTLGenerator::processPropertyReferences(const std::string& content) {
    // 处理引用属性
    // 这里需要实现属性引用解析
    return content;
}


} // namespace CHTL