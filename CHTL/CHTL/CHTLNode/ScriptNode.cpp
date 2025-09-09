#include "ScriptNode.hpp"
#include <sstream>

namespace CHTL {

ScriptNode::ScriptNode(ScriptType type, size_t line, size_t column)
    : BaseNode(NodeType::SCRIPT, line, column), scriptType_(type) {}

void ScriptNode::addCHTLJSCode(const std::string& code) {
    chtljsCode_.push_back(code);
}

void ScriptNode::addJavaScriptCode(const std::string& code) {
    javascriptCode_.push_back(code);
}

void ScriptNode::addEnhancedSelector(const std::string& selector) {
    enhancedSelectors_.push_back(selector);
}

void ScriptNode::addCHTLJSFunction(const std::string& functionName, const std::string& functionCode) {
    chtljsFunctions_[functionName] = functionCode;
}

std::string ScriptNode::toHTML() const {
    std::ostringstream html;
    
    if (scriptType_ == ScriptType::GLOBAL) {
        html << "<script>\n";
    } else {
        html << "<!-- Local Script -->\n";
    }
    
    // 输出CHTL JS代码
    for (const auto& code : chtljsCode_) {
        html << code << "\n";
    }
    
    // 输出JavaScript代码
    for (const auto& code : javascriptCode_) {
        html << code << "\n";
    }
    
    if (scriptType_ == ScriptType::GLOBAL) {
        html << "</script>\n";
    }
    
    return html.str();
}

std::string ScriptNode::toString() const {
    std::ostringstream oss;
    oss << "ScriptNode(" << (scriptType_ == ScriptType::LOCAL ? "LOCAL" : "GLOBAL") 
        << ", " << line_ << ":" << column_ << ")";
    return oss.str();
}

} // namespace CHTL