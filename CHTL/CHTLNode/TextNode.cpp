#include "TextNode.h"
#include <sstream>

namespace CHTL {

std::string TextNode::ToString() const {
    std::stringstream ss;
    ss << "text { \"" << value << "\" }";
    return ss.str();
}

} // namespace CHTL
