#include "StyleNode.h"

namespace CHTL {

StyleNode::StyleNode() {}

NodeType StyleNode::getType() const {
    return NodeType::Style;
}

std::unique_ptr<BaseNode> StyleNode::clone() const {
    auto new_node = std::make_unique<StyleNode>();
    new_node->m_rules = this->m_rules; // CssRule is copyable
    return new_node;
}

void StyleNode::addRule(const CssRule& rule) {
    m_rules.push_back(rule);
}

const std::vector<CssRule>& StyleNode::getRules() const {
    return m_rules;
}

} // namespace CHTL
