#include "CHTLJS/CHTLJSNode/CHTLJSRouterNode.h"

namespace CHTL {

CHTLJSRouterNode::CHTLJSRouterNode() {
}

CHTLJSRouterNode::~CHTLJSRouterNode() = default;

std::string CHTLJSRouterNode::getRoutePath() const {
    return m_routePath;
}

void CHTLJSRouterNode::setRoutePath(const std::string& routePath) {
    m_routePath = routePath;
}

} // namespace CHTL