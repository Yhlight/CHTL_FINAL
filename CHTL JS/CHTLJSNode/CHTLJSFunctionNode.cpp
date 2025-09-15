#include "CHTLJSFunctionNode.h"
#include <sstream>

namespace CHTLJS {

CHTLJSFunctionNode::CHTLJSFunctionNode(NodeType type, const std::string& functionName)
    : CHTLJSBaseNode(type), function_name_(functionName) {
}

void CHTLJSFunctionNode::setFunctionName(const std::string& name) {
    function_name_ = name;
}

const std::string& CHTLJSFunctionNode::getFunctionName() const {
    return function_name_;
}

void CHTLJSFunctionNode::addParameter(const std::string& key, const std::string& value) {
    string_parameters_[key] = value;
}

void CHTLJSFunctionNode::addParameter(const std::string& key, std::shared_ptr<CHTLJSBaseNode> value) {
    node_parameters_[key] = value;
}

const std::map<std::string, std::string>& CHTLJSFunctionNode::getStringParameters() const {
    return string_parameters_;
}

const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& CHTLJSFunctionNode::getNodeParameters() const {
    return node_parameters_;
}

std::string CHTLJSFunctionNode::generateCode() const {
    std::stringstream ss;
    ss << function_name_ << "({";
    
    bool first = true;
    for (const auto& param : string_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": \"" << param.second << "\"";
        first = false;
    }
    
    for (const auto& param : node_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": " << param.second->generateCode();
        first = false;
    }
    
    ss << "})";
    return ss.str();
}

std::string CHTLJSFunctionNode::toString() const {
    std::stringstream ss;
    ss << "CHTLJSFunctionNode(" << function_name_ << ")";
    return ss.str();
}

// ScriptLoaderNode implementation
ScriptLoaderNode::ScriptLoaderNode() 
    : CHTLJSFunctionNode(NodeType::SCRIPT_LOADER, "ScriptLoader") {
}

std::string ScriptLoaderNode::generateCode() const {
    std::stringstream ss;
    ss << "ScriptLoader({";
    
    bool first = true;
    for (const auto& param : string_parameters_) {
        if (!first) ss << ", ";
        ss << "load: \"" << param.second << "\"";
        first = false;
    }
    
    ss << "})";
    return ss.str();
}

// ListenNode implementation
ListenNode::ListenNode() 
    : CHTLJSFunctionNode(NodeType::LISTEN, "Listen") {
}

std::string ListenNode::generateCode() const {
    std::stringstream ss;
    ss << "Listen({";
    
    bool first = true;
    for (const auto& param : string_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": " << param.second;
        first = false;
    }
    
    for (const auto& param : node_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": " << param.second->generateCode();
        first = false;
    }
    
    ss << "})";
    return ss.str();
}

// AnimateNode implementation
AnimateNode::AnimateNode() 
    : CHTLJSFunctionNode(NodeType::ANIMATE, "Animate") {
}

std::string AnimateNode::generateCode() const {
    std::stringstream ss;
    ss << "Animate({";
    
    bool first = true;
    for (const auto& param : string_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": " << param.second;
        first = false;
    }
    
    for (const auto& param : node_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": " << param.second->generateCode();
        first = false;
    }
    
    ss << "})";
    return ss.str();
}

// RouterNode implementation
RouterNode::RouterNode() 
    : CHTLJSFunctionNode(NodeType::ROUTER, "Router") {
}

std::string RouterNode::generateCode() const {
    std::stringstream ss;
    ss << "Router({";
    
    bool first = true;
    for (const auto& param : string_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": " << param.second;
        first = false;
    }
    
    for (const auto& param : node_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": " << param.second->generateCode();
        first = false;
    }
    
    ss << "})";
    return ss.str();
}

// VirNode implementation
VirNode::VirNode() 
    : CHTLJSFunctionNode(NodeType::VIR, "Vir") {
}

std::string VirNode::generateCode() const {
    std::stringstream ss;
    ss << "Vir " << function_name_ << " = ";
    
    if (!node_parameters_.empty()) {
        auto it = node_parameters_.begin();
        ss << it->second->generateCode();
    }
    
    return ss.str();
}

// INeverAwayNode implementation
INeverAwayNode::INeverAwayNode() 
    : CHTLJSFunctionNode(NodeType::INEVERAWAY, "iNeverAway") {
}

std::string INeverAwayNode::generateCode() const {
    std::stringstream ss;
    ss << "iNeverAway({";
    
    bool first = true;
    for (const auto& param : string_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": " << param.second;
        first = false;
    }
    
    for (const auto& param : node_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": " << param.second->generateCode();
        first = false;
    }
    
    ss << "})";
    return ss.str();
}

// UtilThenNode implementation
UtilThenNode::UtilThenNode() 
    : CHTLJSFunctionNode(NodeType::UTIL_THEN, "util") {
}

std::string UtilThenNode::generateCode() const {
    std::stringstream ss;
    ss << "util ";
    
    // 这里需要根据具体的util语法来生成代码
    // 暂时返回基本结构
    ss << "expression -> change { condition } -> then { action }";
    
    return ss.str();
}

// PrintMyloveNode implementation
PrintMyloveNode::PrintMyloveNode() 
    : CHTLJSFunctionNode(NodeType::PRINTMYLOVE, "printMylove") {
}

std::string PrintMyloveNode::generateCode() const {
    std::stringstream ss;
    ss << "printMylove({";
    
    bool first = true;
    for (const auto& param : string_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": " << param.second;
        first = false;
    }
    
    for (const auto& param : node_parameters_) {
        if (!first) ss << ", ";
        ss << param.first << ": " << param.second->generateCode();
        first = false;
    }
    
    ss << "})";
    return ss.str();
}

} // namespace CHTLJS