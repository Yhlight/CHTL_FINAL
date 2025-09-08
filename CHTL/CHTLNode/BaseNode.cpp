#include "BaseNode.h"

namespace CHTL {

void BaseNode::addChild(std::unique_ptr<BaseNode> child) {
    m_children.push_back(std::move(child));
}

const std::vector<std::unique_ptr<BaseNode>>& BaseNode::getChildren() const {
    return m_children;
}

} // namespace CHTL
