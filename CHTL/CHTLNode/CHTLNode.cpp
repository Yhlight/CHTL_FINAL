#include "CHTLNode.h"
#include <sstream>

namespace CHTL {

// CHTLNode 基础实现
void CHTLNode::addChild(std::unique_ptr<CHTLNode> child) {
    children.push_back(std::move(child));
}

void CHTLNode::setAttribute(const std::string& name, const std::string& value) {
    attributes[name] = value;
}

std::string CHTLNode::getAttribute(const std::string& name) const {
    auto it = attributes.find(name);
    if (it != attributes.end()) {
        return it->second;
    }
    return "";
}

bool CHTLNode::hasAttribute(const std::string& name) const {
    return attributes.find(name) != attributes.end();
}

// DocumentNode 实现
std::string DocumentNode::toString() const {
    std::stringstream ss;
    ss << "DocumentNode(" << children.size() << " children)";
    return ss.str();
}

std::string DocumentNode::toHTML() const {
    std::stringstream ss;
    for (const auto& child : children) {
        ss << child->toHTML();
    }
    return ss.str();
}

// ElementNode 实现
std::string ElementNode::toString() const {
    std::stringstream ss;
    ss << "ElementNode(" << tagName << ", " << children.size() << " children)";
    return ss.str();
}

std::string ElementNode::toHTML() const {
    std::stringstream ss;
    ss << "<" << tagName;
    
    // 输出属性
    for (const auto& attr : attributes) {
        ss << " " << attr.name;
        if (!attr.value.empty()) {
            ss << "=\"" << attr.value << "\"";
        }
    }
    
    // 自闭合标签
    if (children.empty() && isSelfClosingTag(tagName)) {
        ss << " />";
    } else {
        ss << ">";
        
        // 输出子节点
        for (const auto& child : children) {
            ss << child->toHTML();
        }
        
        ss << "</" << tagName << ">";
    }
    
    return ss.str();
}

bool ElementNode::isSelfClosingTag(const std::string& tag) const {
    static const std::vector<std::string> selfClosingTags = {
        "img", "br", "hr", "input", "meta", "link", "area", "base", "col", "embed",
        "source", "track", "wbr"
    };
    
    for (const auto& selfClosing : selfClosingTags) {
        if (tag == selfClosing) {
            return true;
        }
    }
    return false;
}

// TextNode 实现
std::string TextNode::toString() const {
    std::stringstream ss;
    ss << "TextNode(\"" << text << "\", literal=" << (isLiteral ? "true" : "false") << ")";
    return ss.str();
}

std::string TextNode::toHTML() const {
    // 转义HTML特殊字符
    std::string result = text;
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

// CommentNode 实现
std::string CommentNode::toString() const {
    std::stringstream ss;
    ss << "CommentNode(\"" << comment << "\", generator=" << (isGeneratorComment ? "true" : "false") << ")";
    return ss.str();
}

std::string CommentNode::toHTML() const {
    if (isGeneratorComment) {
        // 生成器注释根据上下文生成不同语言的注释
        return "<!-- " + comment + " -->";
    } else {
        // 普通注释不输出到HTML
        return "";
    }
}

// StyleNode 实现
std::string StyleNode::toString() const {
    std::stringstream ss;
    ss << "StyleNode(" << rules.size() << " rules, local=" << (isLocal ? "true" : "false") << ")";
    return ss.str();
}

std::string StyleNode::toHTML() const {
    if (rules.empty()) {
        return "";
    }
    
    std::stringstream ss;
    ss << "<style>";
    
    for (const auto& rule : rules) {
        ss << rule.selector << " {";
        for (const auto& prop : rule.properties) {
            ss << prop.first << ": " << prop.second << ";";
        }
        ss << "}";
    }
    
    ss << "</style>";
    return ss.str();
}

// ScriptNode 实现
std::string ScriptNode::toString() const {
    std::stringstream ss;
    ss << "ScriptNode(\"" << script << "\", local=" << (isLocal ? "true" : "false") << ")";
    return ss.str();
}

std::string ScriptNode::toHTML() const {
    if (script.empty()) {
        return "";
    }
    
    std::stringstream ss;
    ss << "<script>" << script << "</script>";
    return ss.str();
}

// TemplateNode 实现
std::string TemplateNode::toString() const {
    std::stringstream ss;
    ss << "TemplateNode(" << name << ", type=" << static_cast<int>(templateType) << ")";
    return ss.str();
}

std::string TemplateNode::toHTML() const {
    // 模板节点不直接输出到HTML，而是被引用时展开
    return "";
}

// CustomNode 实现
std::string CustomNode::toString() const {
    std::stringstream ss;
    ss << "CustomNode(" << name << ", type=" << static_cast<int>(customType) << ")";
    return ss.str();
}

std::string CustomNode::toHTML() const {
    // 自定义节点不直接输出到HTML，而是被引用时展开
    return "";
}

// OriginNode 实现
std::string OriginNode::toString() const {
    std::stringstream ss;
    ss << "OriginNode(" << name << ", type=" << static_cast<int>(originType) << ")";
    return ss.str();
}

std::string OriginNode::toHTML() const {
    // 原始嵌入节点直接输出内容
    return content;
}

// ImportNode 实现
std::string ImportNode::toString() const {
    std::stringstream ss;
    ss << "ImportNode(" << path << ", type=" << static_cast<int>(importType) << ")";
    return ss.str();
}

std::string ImportNode::toHTML() const {
    // 导入节点不直接输出到HTML，而是在编译时处理
    return "";
}

// NamespaceNode 实现
std::string NamespaceNode::toString() const {
    std::stringstream ss;
    ss << "NamespaceNode(" << name << ", " << content.size() << " children)";
    return ss.str();
}

std::string NamespaceNode::toHTML() const {
    // 命名空间节点不直接输出到HTML，而是在编译时处理
    return "";
}

// ConfigurationNode 实现
std::string ConfigurationNode::toString() const {
    std::stringstream ss;
    ss << "ConfigurationNode(" << name << ", " << settings.size() << " settings)";
    return ss.str();
}

std::string ConfigurationNode::toHTML() const {
    // 配置节点不直接输出到HTML，而是在编译时处理
    return "";
}

} // namespace CHTL