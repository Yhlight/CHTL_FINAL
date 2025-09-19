#include "ScriptNode.h"
#include "Visitor.h"

namespace CHTL {

ScriptNode::ScriptNode(const std::string& content) : content(content) {}

void ScriptNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> ScriptNode::clone() const {
    return std::make_unique<ScriptNode>(content);
}

} // namespace CHTL
