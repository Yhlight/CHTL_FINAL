#include "OriginNode.h"
#include <sstream>

namespace CHTL {

OriginNode::OriginNode(const std::string& name)
    : BaseNode(name), originType_("html") {
}

void OriginNode::setOriginContent(const std::string& content) {
    originContent_ = content;
}

void OriginNode::setOriginType(const std::string& type) {
    originType_ = type;
}

std::string OriginNode::generateHTML() const {
    if (originType_ == "html") {
        return originContent_;
    }
    return "";
}

std::string OriginNode::generateCSS() const {
    if (originType_ == "css") {
        return originContent_;
    }
    return "";
}

std::string OriginNode::generateJS() const {
    if (originType_ == "javascript") {
        return originContent_;
    }
    return "";
}

} // namespace CHTL