#include "ScriptNode.h"
#include "Visitor.h"

namespace CHTL {

void ScriptNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> ScriptNode::clone() const {
    return std::make_unique<ScriptNode>(this->content);
}

} // namespace CHTL
