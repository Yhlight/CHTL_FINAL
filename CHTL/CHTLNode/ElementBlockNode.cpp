#include "ElementBlockNode.h"
#include <sstream>

namespace CHTL {

std::string ElementBlockNode::ToString() const {
    std::stringstream ss;
    for (const auto& stmt : statements) {
        ss << stmt->ToString();
    }
    return ss.str();
}

} // namespace CHTL
