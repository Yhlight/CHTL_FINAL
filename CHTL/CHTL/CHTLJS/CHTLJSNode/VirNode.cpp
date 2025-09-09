#include "VirNode.hpp"
#include <sstream>

namespace CHTL {

VirNode::VirNode(const std::string& name, size_t line, size_t column)
    : CHTLJSBaseNode(NodeType::VIR, line, column), name_(name) {}

void VirNode::addKeyValue(const std::string& key, const std::string& value) {
    keyValues_[key] = value;
}

std::string VirNode::toJavaScript() const {
    std::ostringstream oss;
    
    oss << "// Virtual object: " << name_ << "\n";
    oss << "const " << name_ << " = {\n";
    
    for (const auto& [key, value] : keyValues_) {
        oss << "    " << key << ": " << value << ",\n";
    }
    
    oss << "};\n";
    oss << "\n";
    oss << "// Virtual object accessor\n";
    oss << "function get" << name_ << "Property(property) {\n";
    oss << "    if (" << name_ << ".hasOwnProperty(property)) {\n";
    oss << "        const value = " << name_ << "[property];\n";
    oss << "        if (typeof value === 'function') {\n";
    oss << "            return value;\n";
    oss << "        } else if (typeof value === 'object') {\n";
    oss << "            return value;\n";
    oss << "        } else {\n";
    oss << "            return value;\n";
    oss << "        }\n";
    oss << "    }\n";
    oss << "    return undefined;\n";
    oss << "}\n";
    
    return oss.str();
}

std::string VirNode::toString() const {
    std::ostringstream oss;
    oss << "VirNode(name=\"" << name_ 
        << "\", properties=" << keyValues_.size() 
        << ", line=" << getLine() 
        << ", column=" << getColumn() << ")";
    return oss.str();
}

} // namespace CHTL