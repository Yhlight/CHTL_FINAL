#include "EnhancedSelectorNode.hpp"
#include <sstream>
#include <regex>

namespace CHTL {

EnhancedSelectorNode::EnhancedSelectorNode(const std::string& selector, SelectorType type,
                                         size_t line, size_t column)
    : CHTLJSBaseNode(NodeType::ENHANCED_SELECTOR, line, column), selectorType_(type) {
    setValue(selector);
    if (type == SelectorType::AUTO) {
        selectorType_ = parseSelectorType(selector);
    }
}

std::string EnhancedSelectorNode::toJavaScript() const {
    return generateSelectorCode();
}

std::string EnhancedSelectorNode::toString() const {
    std::ostringstream oss;
    std::string typeStr;
    switch (selectorType_) {
        case SelectorType::AUTO: typeStr = "AUTO"; break;
        case SelectorType::CLASS: typeStr = "CLASS"; break;
        case SelectorType::ID: typeStr = "ID"; break;
        case SelectorType::TAG: typeStr = "TAG"; break;
        case SelectorType::DESCENDANT: typeStr = "DESCENDANT"; break;
        case SelectorType::INDEXED: typeStr = "INDEXED"; break;
    }
    
    oss << "EnhancedSelectorNode(type=" << typeStr 
        << ", selector=\"" << getValue() 
        << "\", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

EnhancedSelectorNode::SelectorType EnhancedSelectorNode::parseSelectorType(const std::string& selector) const {
    // 移除 {{ 和 }}
    std::string cleanSelector = selector;
    if (cleanSelector.length() >= 4 && 
        cleanSelector.substr(0, 2) == "{{" && 
        cleanSelector.substr(cleanSelector.length() - 2) == "}}") {
        cleanSelector = cleanSelector.substr(2, cleanSelector.length() - 4);
    }
    
    // 检查索引选择器
    if (std::regex_search(cleanSelector, std::regex(R"(\[\d+\])"))) {
        return SelectorType::INDEXED;
    }
    
    // 检查类选择器
    if (cleanSelector[0] == '.') {
        return SelectorType::CLASS;
    }
    
    // 检查ID选择器
    if (cleanSelector[0] == '#') {
        return SelectorType::ID;
    }
    
    // 检查后代选择器
    if (cleanSelector.find(' ') != std::string::npos) {
        return SelectorType::DESCENDANT;
    }
    
    // 默认为标签选择器
    return SelectorType::TAG;
}

std::string EnhancedSelectorNode::generateSelectorCode() const {
    std::string cleanSelector = getValue();
    
    // 移除 {{ 和 }}
    if (cleanSelector.length() >= 4 && 
        cleanSelector.substr(0, 2) == "{{" && 
        cleanSelector.substr(cleanSelector.length() - 2) == "}}") {
        cleanSelector = cleanSelector.substr(2, cleanSelector.length() - 4);
    }
    
    std::ostringstream oss;
    
    switch (selectorType_) {
        case SelectorType::AUTO: {
            // {{box}} - 先判断是否为tag，然后查找类名/id=box的元素
            oss << "(function() {\n";
            oss << "    // Try as tag first\n";
            oss << "    let elements = document.getElementsByTagName('" << cleanSelector << "');\n";
            oss << "    if (elements.length > 0) return elements;\n";
            oss << "    \n";
            oss << "    // Try as class\n";
            oss << "    elements = document.getElementsByClassName('" << cleanSelector << "');\n";
            oss << "    if (elements.length > 0) return elements;\n";
            oss << "    \n";
            oss << "    // Try as ID\n";
            oss << "    const element = document.getElementById('" << cleanSelector << "');\n";
            oss << "    return element ? [element] : [];\n";
            oss << "})()";
            break;
        }
        
        case SelectorType::CLASS: {
            // {{.box}} - 类选择器
            std::string className = cleanSelector.substr(1); // 移除 .
            oss << "document.getElementsByClassName('" << className << "')";
            break;
        }
        
        case SelectorType::ID: {
            // {{#box}} - ID选择器
            std::string id = cleanSelector.substr(1); // 移除 #
            oss << "document.getElementById('" << id << "')";
            break;
        }
        
        case SelectorType::TAG: {
            // {{button}} - 标签选择器
            oss << "document.getElementsByTagName('" << cleanSelector << "')";
            break;
        }
        
        case SelectorType::DESCENDANT: {
            // {{.box button}} - 后代选择器
            oss << "document.querySelectorAll('" << cleanSelector << "')";
            break;
        }
        
        case SelectorType::INDEXED: {
            // {{button[0]}} - 索引选择器
            std::regex indexRegex(R"((\w+)\[(\d+)\])");
            std::smatch match;
            if (std::regex_match(cleanSelector, match, indexRegex)) {
                std::string tag = match[1].str();
                std::string index = match[2].str();
                oss << "document.getElementsByTagName('" << tag << "')[" << index << "]";
            } else {
                oss << "null";
            }
            break;
        }
    }
    
    return oss.str();
}

} // namespace CHTL