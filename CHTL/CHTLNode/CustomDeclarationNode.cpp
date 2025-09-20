#include "CustomDeclarationNode.h"
#include "Visitor.h"
#include <stdexcept>

namespace CHTL {

void CustomDeclarationNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> CustomDeclarationNode::clone() const {
    // Cloning implementation will be added later
    throw std::logic_error("Cloning for CustomDeclarationNode not implemented yet.");
}

} // namespace CHTL
