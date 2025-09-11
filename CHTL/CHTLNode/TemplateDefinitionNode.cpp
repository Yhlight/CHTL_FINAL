#include "TemplateDefinitionNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void TemplateDefinitionNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
