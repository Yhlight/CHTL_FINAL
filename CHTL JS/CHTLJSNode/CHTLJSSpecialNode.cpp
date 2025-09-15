#include "CHTLJSSpecialNode.h"
#include <sstream>

namespace CHTLJS {

// SelectorNode implementation
SelectorNode::SelectorNode(const std::string& selector)
    : CHTLJSBaseNode(NodeType::SELECTOR), selector_(selector) {
}

void SelectorNode::setSelector(const std::string& selector) {
    selector_ = selector;
}

const std::string& SelectorNode::getSelector() const {
    return selector_;
}

std::string SelectorNode::generateCode() const {
    // 将选择器转换为DOM查询代码
    std::stringstream ss;
    
    if (selector_.empty()) {
        return "null";
    }
    
    // 根据选择器类型生成不同的DOM查询代码
    if (selector_[0] == '.') {
        // 类选择器
        ss << "document.querySelector('" << selector_ << "')";
    } else if (selector_[0] == '#') {
        // ID选择器
        ss << "document.getElementById('" << selector_.substr(1) << "')";
    } else if (selector_.find(' ') != std::string::npos) {
        // 后代选择器
        ss << "document.querySelector('" << selector_ << "')";
    } else if (selector_.find('[') != std::string::npos && selector_.find(']') != std::string::npos) {
        // 精确访问选择器
        ss << "document.querySelectorAll('" << selector_ << "')";
    } else {
        // 标签选择器
        ss << "document.querySelector('" << selector_ << "')";
    }
    
    return ss.str();
}

std::string SelectorNode::toString() const {
    std::stringstream ss;
    ss << "SelectorNode(" << selector_ << ")";
    return ss.str();
}

// ResponsiveValueNode implementation
ResponsiveValueNode::ResponsiveValueNode(const std::string& variableName)
    : CHTLJSBaseNode(NodeType::RESPONSIVE_VALUE), variable_name_(variableName) {
}

void ResponsiveValueNode::setVariableName(const std::string& name) {
    variable_name_ = name;
}

const std::string& ResponsiveValueNode::getVariableName() const {
    return variable_name_;
}

std::string ResponsiveValueNode::generateCode() const {
    return variable_name_;
}

std::string ResponsiveValueNode::toString() const {
    std::stringstream ss;
    ss << "ResponsiveValueNode(" << variable_name_ << ")";
    return ss.str();
}

// ExpressionNode implementation
ExpressionNode::ExpressionNode(const std::string& expression)
    : CHTLJSBaseNode(NodeType::EXPRESSION), expression_(expression) {
}

void ExpressionNode::setExpression(const std::string& expression) {
    expression_ = expression;
}

const std::string& ExpressionNode::getExpression() const {
    return expression_;
}

std::string ExpressionNode::generateCode() const {
    return expression_;
}

std::string ExpressionNode::toString() const {
    std::stringstream ss;
    ss << "ExpressionNode(" << expression_ << ")";
    return ss.str();
}

// BlockNode implementation
BlockNode::BlockNode() : CHTLJSBaseNode(NodeType::BLOCK) {
}

void BlockNode::addStatement(std::shared_ptr<CHTLJSBaseNode> statement) {
    if (statement) {
        statements_.push_back(statement);
    }
}

const std::vector<std::shared_ptr<CHTLJSBaseNode>>& BlockNode::getStatements() const {
    return statements_;
}

std::string BlockNode::generateCode() const {
    std::stringstream ss;
    ss << "{\n";
    
    for (const auto& statement : statements_) {
        ss << "    " << statement->generateCode() << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string BlockNode::toString() const {
    std::stringstream ss;
    ss << "BlockNode(" << statements_.size() << " statements)";
    return ss.str();
}

} // namespace CHTLJS