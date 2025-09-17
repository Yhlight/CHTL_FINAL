#include "CHTLBaseNode.h"
#include <sstream>

namespace CHTL {

CHTLBaseNode::CHTLBaseNode(NodeType type) 
    : type_(type), line_(0), column_(0) {
}

CHTLBaseNode::~CHTLBaseNode() {
}

CHTLBaseNode::NodeType CHTLBaseNode::getType() const {
    return type_;
}

std::string CHTLBaseNode::toString() const {
    std::stringstream ss;
    ss << "CHTLBaseNode(" << static_cast<int>(type_) << ")";
    return ss.str();
}

void CHTLBaseNode::addChild(std::shared_ptr<CHTLBaseNode> child) {
    if (child) {
        children_.push_back(child);
    }
}

const std::vector<std::shared_ptr<CHTLBaseNode>>& CHTLBaseNode::getChildren() const {
    return children_;
}

void CHTLBaseNode::clearChildren() {
    children_.clear();
}

void CHTLBaseNode::setAttribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
}

std::string CHTLBaseNode::getAttribute(const std::string& name) const {
    auto it = attributes_.find(name);
    if (it != attributes_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLBaseNode::hasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

const std::map<std::string, std::string>& CHTLBaseNode::getAttributes() const {
    return attributes_;
}

void CHTLBaseNode::clearAttributes() {
    attributes_.clear();
}

void CHTLBaseNode::setPosition(int line, int column) {
    line_ = line;
    column_ = column;
}

int CHTLBaseNode::getLine() const {
    return line_;
}

int CHTLBaseNode::getColumn() const {
    return column_;
}

} // namespace CHTL