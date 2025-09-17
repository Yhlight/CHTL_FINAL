#include "TemplateUsageNode.h"
#include <sstream>

namespace CHTL {

std::string TemplateUsageNode::ToString() const {
    std::stringstream ss;
    ss << "@" << templateType << " " << name << ";";
    return ss.str();
}

} // namespace CHTL
