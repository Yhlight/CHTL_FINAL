#include "ScriptNode.h"
#include "Visitor.h"

namespace CHTL {

ScriptNode::ScriptNode(const std::string& content) : content(content) {}

void ScriptNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::unique_ptr<BaseNode> ScriptNode::clone() const {
    auto new_node = std::make_unique<ScriptNode>(this->content);
    new_node->placeholder_key = this->placeholder_key;
    return new_node;
}

} // namespace CHTL
