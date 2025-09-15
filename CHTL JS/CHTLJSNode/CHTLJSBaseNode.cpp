#include "CHTLJSBaseNode.h"
#include <sstream>

namespace CHTLJS {

CHTLJSBaseNode::CHTLJSBaseNode(NodeType type) : type_(type) {
}

CHTLJSBaseNode::~CHTLJSBaseNode() {
}

CHTLJSBaseNode::NodeType CHTLJSBaseNode::getType() const {
    return type_;
}

std::string CHTLJSBaseNode::toString() const {
    std::stringstream ss;
    ss << "CHTLJSBaseNode(" << static_cast<int>(type_) << ")";
    return ss.str();
}

void CHTLJSBaseNode::addChild(std::shared_ptr<CHTLJSBaseNode> child) {
    if (child) {
        children_.push_back(child);
    }
}

const std::vector<std::shared_ptr<CHTLJSBaseNode>>& CHTLJSBaseNode::getChildren() const {
    return children_;
}

void CHTLJSBaseNode::clearChildren() {
    children_.clear();
}

} // namespace CHTLJS