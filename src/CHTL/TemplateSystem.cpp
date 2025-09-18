#include "TemplateSystem.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

// Template 实现
Template::Template(TemplateType type, const std::string& name)
    : type_(type), name_(name) {
}

void Template::setContent(const std::string& content) {
    content_ = content;
}

void Template::addParameter(const std::string& name, const std::string& defaultValue) {
    parameters_[name] = defaultValue;
}

std::string Template::getParameter(const std::string& name) const {
    auto it = parameters_.find(name);
    return (it != parameters_.end()) ? it->second : "";
}

bool Template::hasParameter(const std::string& name) const {
    return parameters_.find(name) != parameters_.end();
}

void Template::setParent(const std::string& parentName) {
    parentName_ = parentName;
}

void Template::addConstraint(const std::string& constraint) {
    constraints_.push_back(constraint);
}

std::string Template::render(const std::unordered_map<std::string, std::string>& variables) const {
    std::string result = content_;
    
    // 处理继承
    result = processInheritance(result);
    
    // 处理参数
    result = processParameters(result, variables);
    
    // 处理约束
    result = processConstraints(result);
    
    return result;
}

std::string Template::processInheritance(const std::string& content) const {
    // 处理模板继承
    std::string result = content;
    
    if (hasParent()) {
        // 这里可以添加继承处理逻辑
        // 例如：查找父模板并合并内容
    }
    
    return result;
}

std::string Template::processParameters(const std::string& content, const std::unordered_map<std::string, std::string>& variables) const {
    // 处理模板参数
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

std::string Template::processConstraints(const std::string& content) const {
    // 处理模板约束
    std::string result = content;
    
    // 处理约束条件
    for (const auto& constraint : constraints_) {
        // 这里可以添加约束处理逻辑
        // 例如：根据约束条件过滤内容
    }
    
    return result;
}

// Custom 实现
Custom::Custom(CustomType type, const std::string& name)
    : type_(type), name_(name) {
}

void Custom::setContent(const std::string& content) {
    content_ = content;
}

void Custom::addOperation(const std::string& operation) {
    operations_.push_back(operation);
}

void Custom::setTarget(const std::string& target) {
    target_ = target;
}

void Custom::setCondition(const std::string& condition) {
    condition_ = condition;
}

std::string Custom::apply(const std::string& targetContent) const {
    std::string result = targetContent;
    
    // 处理条件
    if (!condition_.empty() && !processCondition(result)) {
        return result;
    }
    
    // 处理操作
    result = processOperations(result);
    
    return result;
}

std::string Custom::processOperations(const std::string& targetContent) const {
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

std::string Custom::processInsert(const std::string& targetContent) const {
    // 处理插入操作
    std::string result = targetContent;
    
    // 这里可以添加插入逻辑
    // 例如：在指定位置插入内容
    
    return result;
}

std::string Custom::processDelete(const std::string& targetContent) const {
    // 处理删除操作
    std::string result = targetContent;
    
    // 这里可以添加删除逻辑
    // 例如：删除指定内容
    
    return result;
}

std::string Custom::processReplace(const std::string& targetContent) const {
    // 处理替换操作
    std::string result = targetContent;
    
    // 这里可以添加替换逻辑
    // 例如：替换指定内容
    
    return result;
}

bool Custom::processCondition(const std::string& targetContent) const {
    // 处理条件判断
    if (condition_.empty()) {
        return true;
    }
    
    // 这里可以添加条件判断逻辑
    // 例如：检查目标内容是否满足条件
    
    return true;
}

// TemplateSystem 实现
TemplateSystem::TemplateSystem() {
}

void TemplateSystem::registerTemplate(std::shared_ptr<Template> template_) {
    if (template_) {
        templates_[template_->getName()] = template_;
    }
}

std::shared_ptr<Template> TemplateSystem::getTemplate(const std::string& name) const {
    auto it = templates_.find(name);
    return (it != templates_.end()) ? it->second : nullptr;
}

bool TemplateSystem::hasTemplate(const std::string& name) const {
    return templates_.find(name) != templates_.end();
}

void TemplateSystem::removeTemplate(const std::string& name) {
    templates_.erase(name);
}

void TemplateSystem::registerCustom(std::shared_ptr<Custom> custom) {
    if (custom) {
        customs_[custom->getName()] = custom;
    }
}

std::shared_ptr<Custom> TemplateSystem::getCustom(const std::string& name) const {
    auto it = customs_.find(name);
    return (it != customs_.end()) ? it->second : nullptr;
}

bool TemplateSystem::hasCustom(const std::string& name) const {
    return customs_.find(name) != customs_.end();
}

void TemplateSystem::removeCustom(const std::string& name) {
    customs_.erase(name);
}

std::string TemplateSystem::renderTemplate(const std::string& name, const std::unordered_map<std::string, std::string>& variables) const {
    auto template_ = getTemplate(name);
    if (!template_) {
        reportError("Template not found: " + name);
        return "";
    }
    
    return template_->render(variables);
}

std::string TemplateSystem::renderCustom(const std::string& name, const std::string& targetContent) const {
    auto custom = getCustom(name);
    if (!custom) {
        reportError("Custom not found: " + name);
        return targetContent;
    }
    
    return custom->apply(targetContent);
}

std::string TemplateSystem::resolveInheritance(const std::string& templateName) const {
    auto template_ = getTemplate(templateName);
    if (!template_) {
        reportError("Template not found: " + templateName);
        return "";
    }
    
    std::string result = template_->getContent();
    
    // 处理继承链
    std::string currentName = templateName;
    std::unordered_set<std::string> visited;
    
    while (!currentName.empty() && visited.find(currentName) == visited.end()) {
        visited.insert(currentName);
        
        auto currentTemplate = getTemplate(currentName);
        if (!currentTemplate || !currentTemplate->hasParent()) {
            break;
        }
        
        currentName = currentTemplate->getParent();
        auto parentTemplate = getTemplate(currentName);
        if (parentTemplate) {
            // 合并父模板内容
            result = parentTemplate->getContent() + "\n" + result;
        }
    }
    
    return result;
}

bool TemplateSystem::validateTemplate(const std::string& name) const {
    auto template_ = getTemplate(name);
    if (!template_) {
        return false;
    }
    
    // 验证模板内容
    if (template_->getContent().empty()) {
        return false;
    }
    
    // 验证参数
    for (const auto& param : template_->getParameters()) {
        if (param.first.empty()) {
            return false;
        }
    }
    
    // 验证继承
    if (template_->hasParent()) {
        std::string parentName = template_->getParent();
        if (parentName == name) {
            return false; // 不能继承自己
        }
        
        if (!hasTemplate(parentName)) {
            return false; // 父模板不存在
        }
    }
    
    return true;
}

bool TemplateSystem::validateCustom(const std::string& name) const {
    auto custom = getCustom(name);
    if (!custom) {
        return false;
    }
    
    // 验证自定义内容
    if (custom->getContent().empty()) {
        return false;
    }
    
    // 验证操作
    for (const auto& operation : custom->getOperations()) {
        if (operation != "insert" && operation != "delete" && operation != "replace") {
            return false;
        }
    }
    
    return true;
}

void TemplateSystem::setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler) {
    errorHandler_ = handler;
}

void TemplateSystem::clear() {
    templates_.clear();
    customs_.clear();
}

void TemplateSystem::reportError(const std::string& message, size_t line, size_t column) {
    if (errorHandler_) {
        errorHandler_(message, line, column);
    } else {
        std::cerr << "Template System Error at " << line << ":" << column << ": " << message << std::endl;
    }
}

std::string TemplateSystem::processTemplateContent(const std::string& content, const std::unordered_map<std::string, std::string>& variables) const {
    std::string result = content;
    
    // 处理变量替换
    result = replaceVariables(result, variables);
    
    // 处理条件
    result = processConditionals(result);
    
    // 处理循环
    result = processLoops(result);
    
    return result;
}

std::string TemplateSystem::processCustomContent(const std::string& content, const std::string& targetContent) const {
    std::string result = content;
    
    // 处理自定义内容
    // 这里可以添加自定义内容处理逻辑
    
    return result;
}

std::string TemplateSystem::parseTemplateBlock(const std::string& content) const {
    // 解析模板块
    std::string result = content;
    
    // 这里可以添加模板块解析逻辑
    
    return result;
}

std::string TemplateSystem::parseCustomBlock(const std::string& content) const {
    // 解析自定义块
    std::string result = content;
    
    // 这里可以添加自定义块解析逻辑
    
    return result;
}

std::string TemplateSystem::replaceVariables(const std::string& content, const std::unordered_map<std::string, std::string>& variables) const {
    std::string result = content;
    
    // 替换变量占位符
    for (const auto& variable : variables) {
        std::string placeholder = "{{" + variable.first + "}}";
        std::string value = variable.second;
        
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    
    return result;
}

std::string TemplateSystem::processConditionals(const std::string& content) const {
    // 处理条件语句
    std::string result = content;
    
    // 这里可以添加条件处理逻辑
    // 例如：处理 {{#if condition}} ... {{/if}} 语法
    
    return result;
}

std::string TemplateSystem::processLoops(const std::string& content) const {
    // 处理循环语句
    std::string result = content;
    
    // 这里可以添加循环处理逻辑
    // 例如：处理 {{#each items}} ... {{/each}} 语法
    
    return result;
}

} // namespace CHTL