#include "StyleNode.h"
#include <sstream>

namespace CHTL {

std::string StyleNode::ToString() const {
    std::stringstream ss;
    ss << "style { ";
    for (const auto& prop : properties) {
        ss << prop.first << ":" << prop.second << "; ";
    }
    ss << "}";
    return ss.str();
}

} // namespace CHTL
