#include "ScriptNode.h"
#include <sstream>

namespace CHTL {

ScriptNode::ScriptNode(const std::string& name)
    : BaseNode(name), scriptType_("text/javascript") {
}

void ScriptNode::setScriptContent(const std::string& content) {
    scriptContent_ = content;
}

void ScriptNode::setScriptType(const std::string& type) {
    scriptType_ = type;
}

void ScriptNode::setScriptSrc(const std::string& src) {
    scriptSrc_ = src;
}

std::string ScriptNode::generateHTML() const {
    std::ostringstream oss;
    oss << "<" << getName();
    
    if (!scriptSrc_.empty()) {
        oss << " src=\"" << scriptSrc_ << "\"";
    }
    
    if (!scriptType_.empty()) {
        oss << " type=\"" << scriptType_ << "\"";
    }
    
    oss << ">";
    oss << scriptContent_;
    oss << "</" << getName() << ">";
    
    return oss.str();
}

std::string ScriptNode::generateCSS() const {
    return "";
}

std::string ScriptNode::generateJS() const {
    return scriptContent_;
}

} // namespace CHTL