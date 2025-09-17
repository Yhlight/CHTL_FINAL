#include "CHTLASTNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

std::string CHTLASTNode::toString() const {
    std::ostringstream oss;
    oss << "CHTLASTNode{type=" << static_cast<int>(type) 
        << ", token=" << token.toString() << "}";
    return oss.str();
}

void CHTLASTNode::addChild(std::shared_ptr<CHTLASTNode> child) {
    if (child) {
        children.push_back(child);
    }
}

void CHTLASTNode::setProperty(const std::string& key, std::shared_ptr<CHTLASTNode> value) {
    if (value) {
        properties[key] = value;
    }
}

std::shared_ptr<CHTLASTNode> CHTLASTNode::getProperty(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return it->second;
    }
    return nullptr;
}

bool CHTLASTNode::hasChildren() const {
    return !children.empty();
}

size_t CHTLASTNode::getChildCount() const {
    return children.size();
}

std::shared_ptr<CHTLASTNode> CHTLASTNode::getChild(size_t index) const {
    if (index < children.size()) {
        return children[index];
    }
    return nullptr;
}

bool CHTLASTNode::hasProperty(const std::string& key) const {
    return properties.find(key) != properties.end();
}

std::vector<std::string> CHTLASTNode::getPropertyKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : properties) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::string ProgramNode::toHTML() const {
    std::ostringstream html;
    for (const auto& child : children) {
        if (child) {
            html << child->toHTML();
        }
    }
    return html.str();
}

std::string HTMLElementNode::toHTML() const {
    std::ostringstream html;
    
    // 开始标签
    html << "<" << tagName;
    
    // 添加属性
    for (const auto& attr : attributes) {
        if (attr) {
            html << " " << attr->toHTML();
        }
    }
    
    if (isSelfClosing) {
        html << " />";
    } else {
        html << ">";
        
        // 添加子元素
        for (const auto& child : children) {
            if (child) {
                html << child->toHTML();
            }
        }
        
        // 结束标签
        html << "</" << tagName << ">";
    }
    
    return html.str();
}

std::string TextNode::toHTML() const {
    if (isRaw) {
        return content;
    }
    
    // 转义HTML特殊字符
    std::string escaped = content;
    size_t pos = 0;
    while ((pos = escaped.find("&", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&amp;");
        pos += 5;
    }
    pos = 0;
    while ((pos = escaped.find("<", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&lt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = escaped.find(">", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&gt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = escaped.find("\"", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&quot;");
        pos += 6;
    }
    pos = 0;
    while ((pos = escaped.find("'", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return escaped;
}

std::string AttributeNode::toHTML() const {
    if (hasValue) {
        return name + "=\"" + value + "\"";
    } else {
        return name;
    }
}

std::string StyleBlockNode::toHTML() const {
    std::ostringstream css;
    
    if (isGlobal) {
        css << "<style>\n";
    }
    
    for (const auto& rule : rules) {
        if (rule) {
            css << rule->toHTML() << "\n";
        }
    }
    
    if (isGlobal) {
        css << "</style>";
    }
    
    return css.str();
}

std::string StyleRuleNode::toHTML() const {
    std::ostringstream css;
    
    if (selector) {
        css << selector->toHTML() << " {\n";
    }
    
    for (const auto& prop : properties) {
        if (prop) {
            css << "    " << prop->toHTML() << ";\n";
        }
    }
    
    css << "}";
    
    return css.str();
}

std::string StylePropertyNode::toHTML() const {
    std::ostringstream css;
    css << name << ": " << value;
    if (isImportant) {
        css << " !important";
    }
    return css.str();
}

std::string ScriptBlockNode::toHTML() const {
    if (isInline) {
        return "<script type=\"" + type + "\">\n" + content + "\n</script>";
    } else {
        return "<script type=\"" + type + "\" src=\"" + content + "\"></script>";
    }
}

std::string TemplateDeclarationNode::toHTML() const {
    // 模板声明不直接生成HTML，但可以用于调试
    std::ostringstream html;
    html << "<!-- Template: " << templateType << " " << name << " -->";
    return html.str();
}

std::string CustomDeclarationNode::toHTML() const {
    // 自定义声明不直接生成HTML，但可以用于调试
    std::ostringstream html;
    html << "<!-- Custom: " << customType << " " << name << " -->";
    return html.str();
}

std::string OriginNode::toHTML() const {
    if (originType == "@Html") {
        return content;
    } else if (originType == "@Style") {
        return "<style>\n" + content + "\n</style>";
    } else if (originType == "@JavaScript") {
        return "<script>\n" + content + "\n</script>";
    }
    return "";
}

std::string ImportNode::toHTML() const {
    if (importType == "@Html") {
        return "<!-- Import HTML: " + path + " -->";
    } else if (importType == "@Style") {
        return "<link rel=\"stylesheet\" href=\"" + path + "\">";
    } else if (importType == "@JavaScript") {
        return "<script src=\"" + path + "\"></script>";
    }
    return "<!-- Import: " + importType + " " + path + " -->";
}

std::string NamespaceNode::toHTML() const {
    // 命名空间不直接生成HTML，但可以用于调试
    std::ostringstream html;
    html << "<!-- Namespace: " << name << " -->";
    return html.str();
}

std::string ConfigurationNode::toHTML() const {
    // 配置不直接生成HTML，但可以用于调试
    std::ostringstream html;
    html << "<!-- Configuration: " << name << " -->";
    return html.str();
}

std::string InfoNode::toHTML() const {
    // 信息不直接生成HTML，但可以用于调试
    std::ostringstream html;
    html << "<!-- Info: ";
    for (const auto& pair : info) {
        html << pair.first << "=" << pair.second << " ";
    }
    html << "-->";
    return html.str();
}

std::string ExportNode::toHTML() const {
    // 导出不直接生成HTML，但可以用于调试
    std::ostringstream html;
    html << "<!-- Export: ";
    for (const auto& exportName : exports) {
        html << exportName << " ";
    }
    html << "-->";
    return html.str();
}

} // namespace CHTL