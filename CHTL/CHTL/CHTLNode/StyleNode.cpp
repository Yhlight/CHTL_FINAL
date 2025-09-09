#include "StyleNode.hpp"
#include <sstream>
#include <algorithm>

namespace CHTL {

StyleNode::StyleNode(StyleType type, size_t line, size_t column)
    : BaseNode(NodeType::STYLE, line, column), styleType_(type) {}

void StyleNode::addCSSProperty(const std::string& property, const std::string& value) {
    cssProperties_[property] = value;
}

std::string StyleNode::getCSSProperty(const std::string& property) const {
    auto it = cssProperties_.find(property);
    return (it != cssProperties_.end()) ? it->second : "";
}

bool StyleNode::hasCSSProperty(const std::string& property) const {
    return cssProperties_.find(property) != cssProperties_.end();
}

void StyleNode::addSelector(const std::string& selector) {
    selectors_.push_back(selector);
}

bool StyleNode::isInlineStyle() const {
    return styleType_ == StyleType::LOCAL && selectors_.empty();
}

std::string StyleNode::toHTML() const {
    if (styleType_ == StyleType::LOCAL && isInlineStyle()) {
        // 内联样式
        std::ostringstream oss;
        oss << "style=\"";
        bool first = true;
        for (const auto& prop : cssProperties_) {
            if (!first) oss << "; ";
            oss << prop.first << ": " << prop.second;
            first = false;
        }
        oss << "\"";
        return oss.str();
    } else {
        // 样式块
        std::ostringstream oss;
        oss << "<style>";
        oss << toCSS();
        oss << "</style>";
        return oss.str();
    }
}

std::string StyleNode::toCSS() const {
    std::ostringstream oss;
    
    if (selectors_.empty()) {
        // 没有选择器，直接输出属性
        for (const auto& prop : cssProperties_) {
            oss << prop.first << ": " << prop.second << ";\n";
        }
    } else {
        // 有选择器，生成规则
        for (const auto& selector : selectors_) {
            oss << selector << " {\n";
            for (const auto& prop : cssProperties_) {
                oss << "    " << prop.first << ": " << prop.second << ";\n";
            }
            oss << "}\n";
        }
    }
    
    return oss.str();
}

std::string StyleNode::toString() const {
    std::ostringstream oss;
    oss << "StyleNode(type=" << static_cast<int>(styleType_)
        << ", properties=" << cssProperties_.size()
        << ", selectors=" << selectors_.size()
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

std::string StyleNode::generateCSSRules() const {
    return toCSS();
}

} // namespace CHTL