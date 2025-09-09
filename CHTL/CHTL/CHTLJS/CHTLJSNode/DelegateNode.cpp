#include "DelegateNode.hpp"
#include <sstream>

namespace CHTL {

DelegateNode::DelegateNode(size_t line, size_t column)
    : CHTLJSBaseNode(NodeType::DELEGATE, line, column) {}

void DelegateNode::setTarget(const std::string& target) {
    targets_.clear();
    targets_.push_back(target);
}

void DelegateNode::addTarget(const std::string& target) {
    targets_.push_back(target);
}

void DelegateNode::addEventDelegate(const std::string& eventType, const std::string& handler) {
    eventDelegates_[eventType] = handler;
}

std::string DelegateNode::toJavaScript() const {
    std::ostringstream oss;
    
    if (targets_.empty() || eventDelegates_.empty()) {
        return "// No delegation targets or events defined";
    }
    
    oss << "// Event delegation\n";
    oss << "(function() {\n";
    oss << "    const targets = [\n";
    
    for (size_t i = 0; i < targets_.size(); ++i) {
        oss << "        \"" << targets_[i] << "\"";
        if (i < targets_.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "    ];\n";
    oss << "    \n";
    oss << "    // Create global registry for event delegation\n";
    oss << "    if (!window.CHTLDelegationRegistry) {\n";
    oss << "        window.CHTLDelegationRegistry = new Map();\n";
    oss << "    }\n";
    oss << "    \n";
    oss << "    targets.forEach(targetSelector => {\n";
    oss << "        const parentElement = document.querySelector(targetSelector);\n";
    oss << "        if (!parentElement) return;\n";
    oss << "        \n";
    oss << "        const registryKey = targetSelector;\n";
    oss << "        if (window.CHTLDelegationRegistry.has(registryKey)) {\n";
    oss << "            // Merge with existing delegation\n";
    oss << "            const existing = window.CHTLDelegationRegistry.get(registryKey);\n";
    oss << "            Object.assign(existing.events, {\n";
    
    for (const auto& [eventType, handler] : eventDelegates_) {
        oss << "                '" << eventType << "': " << handler << ",\n";
    }
    
    oss << "            });\n";
    oss << "        } else {\n";
    oss << "            // Create new delegation\n";
    oss << "            const delegation = {\n";
    oss << "                parent: parentElement,\n";
    oss << "                events: {\n";
    
    for (const auto& [eventType, handler] : eventDelegates_) {
        oss << "                    '" << eventType << "': " << handler << ",\n";
    }
    
    oss << "                }\n";
    oss << "            };\n";
    oss << "            \n";
    oss << "            // Add event listener to parent\n";
    oss << "            parentElement.addEventListener('" << eventDelegates_.begin()->first << "', (e) => {\n";
    oss << "                const target = e.target;\n";
    oss << "                const eventType = e.type;\n";
    oss << "                \n";
    oss << "                if (delegation.events[eventType]) {\n";
    oss << "                    delegation.events[eventType](e);\n";
    oss << "                }\n";
    oss << "            });\n";
    oss << "            \n";
    oss << "            window.CHTLDelegationRegistry.set(registryKey, delegation);\n";
    oss << "        }\n";
    oss << "    });\n";
    oss << "})();\n";
    
    return oss.str();
}

std::string DelegateNode::toString() const {
    std::ostringstream oss;
    oss << "DelegateNode(targets=[" << targets_.size() << "], events=[" 
        << eventDelegates_.size() << "], line=" << getLine() 
        << ", column=" << getColumn() << ")";
    return oss.str();
}

} // namespace CHTL