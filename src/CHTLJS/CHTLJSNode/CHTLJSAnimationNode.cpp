#include "CHTLJS/CHTLJSNode/CHTLJSAnimationNode.h"

namespace CHTL {

CHTLJSAnimationNode::CHTLJSAnimationNode() {
}

CHTLJSAnimationNode::~CHTLJSAnimationNode() = default;

std::string CHTLJSAnimationNode::getAnimationName() const {
    return m_animationName;
}

void CHTLJSAnimationNode::setAnimationName(const std::string& animationName) {
    m_animationName = animationName;
}

} // namespace CHTL