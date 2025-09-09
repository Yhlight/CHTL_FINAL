#include "EnhancedSelectorNode.hpp"
#include <sstream>
#include <regex>

namespace CHTL {

EnhancedSelectorNode::EnhancedSelectorNode(const std::string& selector, size_t line, size_t column)
    : CHTLJSBaseNode(NodeType::ENHANCED_SELECTOR, line, column), selector_(selector) {}

bool EnhancedSelectorNode::isPreciseAccess() const {
    return selector_.find('[') != std::string::npos && selector_.find(']') != std::string::npos;
}

int EnhancedSelectorNode::getPreciseIndex() const {
    if (!isPreciseAccess()) return -1;
    
    size_t start = selector_.find('[');
    size_t end = selector_.find(']');
    if (start != std::string::npos && end != std::string::npos) {
        std::string indexStr = selector_.substr(start + 1, end - start - 1);
        try {
            return std::stoi(indexStr);
        } catch (...) {
            return -1;
        }
    }
    return -1;
}

std::string EnhancedSelectorNode::toJavaScript() const {
    std::ostringstream oss;
    
    if (isPreciseAccess()) {
        int index = getPreciseIndex();
        std::string baseSelector = selector_.substr(0, selector_.find('['));
        oss << "document.querySelectorAll('" << baseSelector << "')[" << index << "]";
    } else {
        oss << "document.querySelector('" << selector_ << "')";
    }
    
    return oss.str();
}

std::string EnhancedSelectorNode::toString() const {
    std::ostringstream oss;
    oss << "EnhancedSelectorNode(selector=\"" << selector_ << "\""
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

std::string EnhancedSelectorNode::parseSelectorType() const {
    if (selector_.empty()) return "unknown";
    
    if (selector_[0] == '.') return "class";
    if (selector_[0] == '#') return "id";
    if (std::isalpha(selector_[0])) return "tag";
    
    return "unknown";
}

std::string EnhancedSelectorNode::generateDOMQuery() const {
    return toJavaScript();
}

} // namespace CHTL