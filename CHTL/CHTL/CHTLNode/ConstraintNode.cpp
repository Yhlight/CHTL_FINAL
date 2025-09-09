#include "ConstraintNode.hpp"
#include <sstream>
#include <algorithm>

namespace CHTL {

ConstraintNode::ConstraintNode(ConstraintType type, size_t line, size_t column)
    : BaseNode(NodeType::OPERATOR, line, column), constraintType_(type) {}

void ConstraintNode::addPreciseConstraint(const std::string& constraint) {
    preciseConstraints_.push_back(constraint);
}

bool ConstraintNode::isPreciseConstrained(const std::string& element) const {
    return std::find(preciseConstraints_.begin(), preciseConstraints_.end(), element) 
           != preciseConstraints_.end();
}

void ConstraintNode::addTypeConstraint(const std::string& constraint) {
    typeConstraints_.push_back(constraint);
}

bool ConstraintNode::isTypeConstrained(const std::string& type) const {
    return std::find(typeConstraints_.begin(), typeConstraints_.end(), type) 
           != typeConstraints_.end();
}

void ConstraintNode::addGlobalConstraint(const std::string& constraint) {
    globalConstraints_.push_back(constraint);
}

bool ConstraintNode::isGlobalConstrained(const std::string& constraint) const {
    return std::find(globalConstraints_.begin(), globalConstraints_.end(), constraint) 
           != globalConstraints_.end();
}

bool ConstraintNode::validateElement(const std::string& element) const {
    // 检查精确约束
    if (isPreciseConstrained(element)) {
        return false; // 违反约束
    }
    
    // 检查类型约束
    if (element == "div" && isTypeConstrained("@Html")) {
        return false; // 禁止HTML元素
    }
    
    if (element.find("[Custom]") != std::string::npos && isTypeConstrained("[Custom]")) {
        return false; // 禁止自定义类型
    }
    
    if (element.find("[Template]") != std::string::npos && isTypeConstrained("[Template]")) {
        return false; // 禁止模板类型
    }
    
    return true; // 通过验证
}

bool ConstraintNode::validateType(const std::string& type) const {
    // 检查类型约束
    if (isTypeConstrained(type)) {
        return false; // 违反约束
    }
    
    return true; // 通过验证
}

std::string ConstraintNode::toHTML() const {
    // 约束节点不直接生成 HTML
    return "";
}

std::string ConstraintNode::toString() const {
    std::ostringstream oss;
    oss << "ConstraintNode(type=" << static_cast<int>(constraintType_)
        << ", precise=" << preciseConstraints_.size()
        << ", type=" << typeConstraints_.size()
        << ", global=" << globalConstraints_.size()
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

std::vector<std::string> ConstraintNode::parseConstraintString(const std::string& constraintStr) const {
    std::vector<std::string> constraints;
    std::string current;
    
    for (char c : constraintStr) {
        if (c == ',') {
            if (!current.empty()) {
                constraints.push_back(current);
                current.clear();
            }
        } else if (c != ' ') {
            current += c;
        }
    }
    
    if (!current.empty()) {
        constraints.push_back(current);
    }
    
    return constraints;
}

} // namespace CHTL