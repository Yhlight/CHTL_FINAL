#include "TemplateDeclarationNode.h"
#include "Visitor.h"
#include <stdexcept>

namespace CHTL {

void TemplateDeclarationNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> TemplateDeclarationNode::clone() const {
    // Cloning implementation will be added later
    throw std::logic_error("Cloning for TemplateDeclarationNode not implemented yet.");
}

} // namespace CHTL
