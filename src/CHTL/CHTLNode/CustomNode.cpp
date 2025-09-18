#include "CustomNode.h"
#include <sstream>
#include <regex>

namespace CHTL {

CustomNode::CustomNode(const std::string& name, const std::string& type)
    : BaseNode(name), customType_(type) {
}

void CustomNode::setCustomContent(const std::string& content) {
    customContent_ = content;
}

void CustomNode::addOperation(const std::string& operation) {
    operations_.push_back(operation);
}

void CustomNode::setTarget(const std::string& target) {
    target_ = target;
}

void CustomNode::setCondition(const std::string& condition) {
    condition_ = condition;
}

std::string CustomNode::apply(const std::string& targetContent) const {
    std::string result = targetContent;
    
    // 处理条件
    if (!condition_.empty() && !processCondition(result)) {
        return result;
    }
    
    // 处理操作
    result = processOperations(result);
    
    return result;
}

std::string CustomNode::generateHTML() const {
    return customContent_;
}

std::string CustomNode::generateCSS() const {
    if (customType_ == "Style") {
        return customContent_;
    }
    return "";
}

std::string CustomNode::generateJS() const {
    if (customType_ == "Var") {
        return customContent_;
    }
    return "";
}

std::string CustomNode::processOperations(const std::string& targetContent) const {
    std::string result = targetContent;
    
    for (const auto& operation : operations_) {
        if (operation == "insert") {
            result = processInsert(result);
        } else if (operation == "delete") {
            result = processDelete(result);
        } else if (operation == "replace") {
            result = processReplace(result);
        }
    }
    
    return result;
}

std::string CustomNode::processInsert(const std::string& targetContent) const {
    // 处理插入操作
    std::string result = targetContent;
    
    // 这里可以添加插入逻辑
    // 例如：在指定位置插入内容
    
    return result;
}

std::string CustomNode::processDelete(const std::string& targetContent) const {
    // 处理删除操作
    std::string result = targetContent;
    
    // 这里可以添加删除逻辑
    // 例如：删除指定内容
    
    return result;
}

std::string CustomNode::processReplace(const std::string& targetContent) const {
    // 处理替换操作
    std::string result = targetContent;
    
    // 这里可以添加替换逻辑
    // 例如：替换指定内容
    
    return result;
}

bool CustomNode::processCondition(const std::string& targetContent) const {
    // 处理条件判断
    if (condition_.empty()) {
        return true;
    }
    
    // 这里可以添加条件判断逻辑
    // 例如：检查目标内容是否满足条件
    
    return true;
}

} // namespace CHTL