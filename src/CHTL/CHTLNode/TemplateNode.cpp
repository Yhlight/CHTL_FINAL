#include "TemplateNode.h"
#include <sstream>
#include <regex>

namespace CHTL {

TemplateNode::TemplateNode(const std::string& name, const std::string& type)
    : BaseNode(name), templateType_(type) {
}

void TemplateNode::setTemplateContent(const std::string& content) {
    templateContent_ = content;
}

void TemplateNode::addParameter(const std::string& name, const std::string& value) {
    parameters_[name] = value;
}

std::string TemplateNode::getParameter(const std::string& name) const {
    auto it = parameters_.find(name);
    return (it != parameters_.end()) ? it->second : "";
}

bool TemplateNode::hasParameter(const std::string& name) const {
    return parameters_.find(name) != parameters_.end();
}

void TemplateNode::setParent(const std::string& parent) {
    parent_ = parent;
}

void TemplateNode::addConstraint(const std::string& constraint) {
    constraints_.push_back(constraint);
}

std::string TemplateNode::render(const std::unordered_map<std::string, std::string>& variables) const {
    std::string result = templateContent_;
    
    // 处理参数
    result = processParameters(result, variables);
    
    // 处理继承
    result = processInheritance(result);
    
    // 处理约束
    result = processConstraints(result);
    
    return result;
}

std::string TemplateNode::generateHTML() const {
    return render();
}

std::string TemplateNode::generateCSS() const {
    if (templateType_ == "Style") {
        return render();
    }
    return "";
}

std::string TemplateNode::generateJS() const {
    if (templateType_ == "Var") {
        return render();
    }
    return "";
}

std::string TemplateNode::processParameters(const std::string& content, const std::unordered_map<std::string, std::string>& variables) const {
    std::string result = content;
    
    // 替换参数占位符
    for (const auto& param : parameters_) {
        std::string placeholder = "{{" + param.first + "}}";
        std::string value = variables.find(param.first) != variables.end() ? 
                           variables.at(param.first) : param.second;
        
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    
    return result;
}

std::string TemplateNode::processInheritance(const std::string& content) const {
    // 处理模板继承
    std::string result = content;
    
    if (hasParent()) {
        // 这里可以添加继承处理逻辑
        // 例如：查找父模板并合并内容
    }
    
    return result;
}

std::string TemplateNode::processConstraints(const std::string& content) const {
    // 处理模板约束
    std::string result = content;
    
    // 处理约束条件
    for (const auto& constraint : constraints_) {
        // 这里可以添加约束处理逻辑
        // 例如：根据约束条件过滤内容
    }
    
    return result;
}

} // namespace CHTL