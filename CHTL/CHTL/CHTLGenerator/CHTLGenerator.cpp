#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <sstream>

namespace CHTL {

CHTLGenerator::CHTLGenerator() 
    : m_defaultStructure(false), m_inlineCSS(false), m_inlineJavaScript(false) {
}

CHTLGenerator::~CHTLGenerator() {
}

std::string CHTLGenerator::generateHTML(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return "";
    }
    
    m_globalCSS.clear();
    m_globalJavaScript.clear();
    
    std::string html = generateElementHTML(root);
    
    if (m_defaultStructure) {
        html = generateDefaultHTMLStructure(html);
    }
    
    return html;
}

std::string CHTLGenerator::generateCSS(std::shared_ptr<BaseNode> root) {
    return m_globalCSS;
}

std::string CHTLGenerator::generateJavaScript(std::shared_ptr<BaseNode> root) {
    return m_globalJavaScript;
}

void CHTLGenerator::setDefaultStructure(bool enable) {
    m_defaultStructure = enable;
}

void CHTLGenerator::setInlineCSS(bool enable) {
    m_inlineCSS = enable;
}

void CHTLGenerator::setInlineJavaScript(bool enable) {
    m_inlineJavaScript = enable;
}

std::string CHTLGenerator::generateElementHTML(std::shared_ptr<BaseNode> element) {
    if (!element || element->getType() != BaseNode::NodeType::ELEMENT) {
        return "";
    }
    
    auto elementNode = std::static_pointer_cast<ElementNode>(element);
    std::string tagName = elementNode->getTagName();
    
    std::stringstream html;
    html << "<" << tagName;
    
    // 添加属性
    for (const auto& child : elementNode->getChildren()) {
        if (child->getType() == BaseNode::NodeType::TEXT) {
            // 处理属性
            std::string attrText = child->getValue();
            size_t colonPos = attrText.find(':');
            if (colonPos != std::string::npos) {
                std::string attrName = attrText.substr(0, colonPos);
                std::string attrValue = attrText.substr(colonPos + 1);
                // 去除空白字符
                attrName.erase(0, attrName.find_first_not_of(" \t"));
                attrName.erase(attrName.find_last_not_of(" \t") + 1);
                attrValue.erase(0, attrValue.find_first_not_of(" \t"));
                attrValue.erase(attrValue.find_last_not_of(" \t") + 1);
                html << " " << attrName << "=\"" << escapeHTML(attrValue) << "\"";
            }
        }
    }
    
    if (elementNode->isSelfClosing()) {
        html << " />";
    } else {
        html << ">";
        
        // 添加子节点
        for (const auto& child : elementNode->getChildren()) {
            if (child->getType() == BaseNode::NodeType::ELEMENT) {
                html << generateElementHTML(child);
            } else if (child->getType() == BaseNode::NodeType::TEXT) {
                html << generateTextHTML(child);
            } else if (child->getType() == BaseNode::NodeType::COMMENT) {
                html << generateCommentHTML(child);
            } else if (child->getType() == BaseNode::NodeType::STYLE) {
                html << generateStyleCSS(child);
            } else if (child->getType() == BaseNode::NodeType::SCRIPT) {
                html << generateScriptJavaScript(child);
            }
        }
        
        html << "</" << tagName << ">";
    }
    
    return html.str();
}

std::string CHTLGenerator::generateTextHTML(std::shared_ptr<BaseNode> text) {
    if (!text || text->getType() != BaseNode::NodeType::TEXT) {
        return "";
    }
    
    return escapeHTML(text->getValue());
}

std::string CHTLGenerator::generateCommentHTML(std::shared_ptr<BaseNode> comment) {
    if (!comment || comment->getType() != BaseNode::NodeType::COMMENT) {
        return "";
    }
    
    return "<!-- " + comment->getValue() + " -->";
}

// 这些方法已经在头文件中声明为private，这里不需要实现

std::string CHTLGenerator::generateStyleCSS(std::shared_ptr<BaseNode> style) {
    // 生成CSS的简化实现
    return "/* CSS styles */";
}

std::string CHTLGenerator::generateScriptJavaScript(std::shared_ptr<BaseNode> script) {
    // 生成JavaScript的简化实现
    return "// JavaScript code";
}

std::string CHTLGenerator::escapeHTML(const std::string& text) {
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

std::string CHTLGenerator::generateDefaultHTMLStructure(const std::string& content) {
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html lang=\"zh-CN\">\n";
    html << "<head>\n";
    html << "    <meta charset=\"UTF-8\">\n";
    html << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "    <title>CHTL Generated Page</title>\n";
    
    if (!m_globalCSS.empty() && m_inlineCSS) {
        html << "    <style>\n" << m_globalCSS << "    </style>\n";
    } else if (!m_globalCSS.empty()) {
        html << "    <link rel=\"stylesheet\" href=\"style.css\">\n";
    }
    
    html << "</head>\n";
    html << "<body>\n";
    html << content;
    html << "\n</body>\n";
    html << "</html>";
    
    return html.str();
}

} // namespace CHTL