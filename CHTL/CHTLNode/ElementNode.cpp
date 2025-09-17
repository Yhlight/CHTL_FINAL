#include "ElementNode.h"
#include <sstream>

namespace CHTL {

std::string ElementNode::ToString() const {
    std::stringstream ss;
    ss << tagName;
    for (const auto& attr_pair : attributes) {
        ss << " " << attr_pair.first << ":\"" << attr_pair.second << "\"";
    }
    ss << " { ";
    for (const auto& child : children) {
        ss << child->ToString();
    }
    ss << " }";
    return ss.str();
}

} // namespace CHTL
