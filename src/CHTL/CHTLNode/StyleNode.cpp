#include "StyleNode.h"
#include <sstream>

namespace CHTL {

StyleNode::StyleNode(const std::string& name)
    : BaseNode(name), styleType_("text/css") {
}

void StyleNode::setStyleContent(const std::string& content) {
    styleContent_ = content;
}

void StyleNode::setStyleType(const std::string& type) {
    styleType_ = type;
}

std::string StyleNode::generateHTML() const {
    std::ostringstream oss;
    oss << "<" << getName();
    
    if (!styleType_.empty()) {
        oss << " type=\"" << styleType_ << "\"";
    }
    
    oss << ">";
    oss << styleContent_;
    oss << "</" << getName() << ">";
    
    return oss.str();
}

std::string StyleNode::generateCSS() const {
    return styleContent_;
}

std::string StyleNode::generateJS() const {
    return "";
}

} // namespace CHTL