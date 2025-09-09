#include "CHTLJS/CHTLJSNode/CHTLJSSelectorNode.h"

namespace CHTL {

CHTLJSSelectorNode::CHTLJSSelectorNode() {
}

CHTLJSSelectorNode::~CHTLJSSelectorNode() = default;

std::string CHTLJSSelectorNode::getSelector() const {
    return m_selector;
}

void CHTLJSSelectorNode::setSelector(const std::string& selector) {
    m_selector = selector;
}

} // namespace CHTL