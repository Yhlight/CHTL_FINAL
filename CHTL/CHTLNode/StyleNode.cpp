#include "StyleNode.h"
#include <sstream>

namespace CHTL {

std::string StyleNode::ToString() const {
    std::stringstream ss;
    ss << "style { ";
    for (const auto& item : items) {
        ss << item->ToString() << " ";
    }
    ss << "}";
    return ss.str();
}

} // namespace CHTL
