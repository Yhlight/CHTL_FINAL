#include "TemplateUsageNode.hpp"
#include "AstVisitor.hpp"

namespace CHTL {

void TemplateUsageNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL
