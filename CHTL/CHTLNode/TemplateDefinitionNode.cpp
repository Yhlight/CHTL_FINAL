#include "TemplateDefinitionNode.h"
#include <sstream>

namespace CHTL {

std::string TemplateDefinitionNode::ToString() const {
    std::stringstream ss;
    ss << "[Template] @" << templateType << " " << name << " { ";
    if (body) {
        ss << body->ToString();
    }
    ss << " }";
    return ss.str();
}

} // namespace CHTL
