#include "ListenNode.hpp"
#include <sstream>

namespace CHTL {

ListenNode::ListenNode(size_t line, size_t column)
    : CHTLJSBaseNode(NodeType::LISTEN, line, column) {}

void ListenNode::addEventListener(const std::string& eventType, const std::string& handler) {
    eventListeners_[eventType] = handler;
}

std::string ListenNode::toJavaScript() const {
    std::ostringstream oss;
    
    if (eventListeners_.empty()) {
        return "// No event listeners defined";
    }
    
    oss << "// Event listeners\n";
    for (const auto& [eventType, handler] : eventListeners_) {
        oss << "element.addEventListener('" << eventType << "', " << handler << ");\n";
    }
    
    return oss.str();
}

std::string ListenNode::toString() const {
    std::ostringstream oss;
    oss << "ListenNode(events=[" << eventListeners_.size() << "], line=" << getLine() 
        << ", column=" << getColumn() << ")";
    return oss.str();
}

} // namespace CHTL