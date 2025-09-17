#include "StylePropertyNode.h"
#include <sstream>

namespace CHTL {

std::string StylePropertyNode::ToString() const {
    std::stringstream ss;
    ss << key << ":" << value << ";";
    return ss.str();
}

} // namespace CHTL
