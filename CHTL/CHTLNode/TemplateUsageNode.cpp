#include "TemplateUsageNode.h"
#include "Visitor.h"

namespace CHTL {

TemplateUsageNode::TemplateUsageNode(const std::string& name, const std::string& from)
    : template_name(name), from_namespace(from) {}

void TemplateUsageNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> TemplateUsageNode::clone() const {
    return std::make_unique<TemplateUsageNode>(template_name, from_namespace);
}

} // namespace CHTL
