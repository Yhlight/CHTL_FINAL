#include "TemplateNode.h"
#include "BaseNode.h"
#include "ElementNode.h"
#include "TextNode.h"
#include "CommentNode.h"
#include "StyleNode.h"
#include "ScriptNode.h"
#include "CustomNode.h"
#include "OriginNode.h"
#include "ImportNode.h"
#include "NamespaceNode.h"
#include "ConstraintNode.h"
#include "ConfigurationNode.h"
#include "UseNode.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <memory>
#include <fstream>
#include <filesystem>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <stdexcept>
#include <exception>
#include <typeinfo>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <random>
#include <cmath>
#include <climits>
#include <cfloat>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <locale>
#include <codecvt>
#include <iomanip>

namespace CHTL {

TemplateNode::TemplateNode(const std::string& name, TemplateType type)
    : BaseNode(NodeType::TEMPLATE, name), templateType(type), templateName(name), 
      isAbstract(false), isFinal(false), priority(0) {
}

std::string TemplateNode::getTemplateTypeName() const {
    switch (templateType) {
        case TemplateType::STYLE: return "Style";
        case TemplateType::ELEMENT: return "Element";
        case TemplateType::VARIABLE: return "Variable";
        case TemplateType::MIXED: return "Mixed";
        default: return "Unknown";
    }
}

void TemplateNode::setParameter(const std::string& name, const std::string& value) {
    parameters[name] = value;
}

std::string TemplateNode::getParameter(const std::string& name) const {
    auto it = parameters.find(name);
    return (it != parameters.end()) ? it->second : "";
}

bool TemplateNode::hasParameter(const std::string& name) const {
    return parameters.find(name) != parameters.end();
}

void TemplateNode::removeParameter(const std::string& name) {
    parameters.erase(name);
}

void TemplateNode::addTemplateContent(std::shared_ptr<BaseNode> content) {
    if (content) {
        templateContent.push_back(content);
    }
}

void TemplateNode::removeTemplateContent(std::shared_ptr<BaseNode> content) {
    auto it = std::find(templateContent.begin(), templateContent.end(), content);
    if (it != templateContent.end()) {
        templateContent.erase(it);
    }
}

void TemplateNode::addParentTemplate(const std::string& parentName) {
    if (std::find(parentTemplates.begin(), parentTemplates.end(), parentName) == parentTemplates.end()) {
        parentTemplates.push_back(parentName);
    }
}

void TemplateNode::removeParentTemplate(const std::string& parentName) {
    auto it = std::find(parentTemplates.begin(), parentTemplates.end(), parentName);
    if (it != parentTemplates.end()) {
        parentTemplates.erase(it);
    }
}

bool TemplateNode::hasParentTemplate(const std::string& parentName) const {
    return std::find(parentTemplates.begin(), parentTemplates.end(), parentName) != parentTemplates.end();
}

void TemplateNode::setInheritedProperty(const std::string& name, const std::string& value) {
    inheritedProperties[name] = value;
}

std::string TemplateNode::getInheritedProperty(const std::string& name) const {
    auto it = inheritedProperties.find(name);
    return (it != inheritedProperties.end()) ? it->second : "";
}

bool TemplateNode::hasInheritedProperty(const std::string& name) const {
    return inheritedProperties.find(name) != inheritedProperties.end();
}

void TemplateNode::removeInheritedProperty(const std::string& name) {
    inheritedProperties.erase(name);
}

void TemplateNode::addConstraint(const std::string& constraint) {
    if (std::find(constraints.begin(), constraints.end(), constraint) == constraints.end()) {
        constraints.push_back(constraint);
    }
}

void TemplateNode::removeConstraint(const std::string& constraint) {
    auto it = std::find(constraints.begin(), constraints.end(), constraint);
    if (it != constraints.end()) {
        constraints.erase(it);
    }
}

bool TemplateNode::hasConstraint(const std::string& constraint) const {
    return std::find(constraints.begin(), constraints.end(), constraint) != constraints.end();
}

void TemplateNode::setTypeConstraint(const std::string& type, const std::string& constraint) {
    typeConstraints[type] = constraint;
}

std::string TemplateNode::getTypeConstraint(const std::string& type) const {
    auto it = typeConstraints.find(type);
    return (it != typeConstraints.end()) ? it->second : "";
}

bool TemplateNode::hasTypeConstraint(const std::string& type) const {
    return typeConstraints.find(type) != typeConstraints.end();
}

void TemplateNode::removeTypeConstraint(const std::string& type) {
    typeConstraints.erase(type);
}

void TemplateNode::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string TemplateNode::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    return (it != metadata.end()) ? it->second : "";
}

bool TemplateNode::hasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

void TemplateNode::removeMetadata(const std::string& key) {
    metadata.erase(key);
}

std::shared_ptr<BaseNode> TemplateNode::instantiate(const std::map<std::string, std::string>& args) const {
    switch (templateType) {
        case TemplateType::STYLE:
            return instantiateStyleTemplate(args);
        case TemplateType::ELEMENT:
            return instantiateElementTemplate(args);
        case TemplateType::VARIABLE:
            return instantiateVariableTemplate(args);
        case TemplateType::MIXED:
            // 混合模板需要特殊处理
            return instantiateElementTemplate(args);
        default:
            return nullptr;
    }
}

std::shared_ptr<BaseNode> TemplateNode::instantiateStyleTemplate(const std::map<std::string, std::string>& args) const {
    auto styleNode = std::make_shared<StyleNode>(false); // 全局样式
    
    // 处理模板内容
    for (const auto& content : templateContent) {
        if (content->getNodeType() == NodeType::STYLE) {
            auto styleContent = std::static_pointer_cast<StyleNode>(content);
            
            // 复制样式属性
            for (const auto& prop : styleContent->getStyleProperties()) {
                std::string processedValue = processTemplateString(prop.second, args);
                styleNode->setStyleProperty(prop.first, processedValue);
            }
            
            // 复制样式规则
            for (const auto& rule : styleContent->getStyleRules()) {
                styleNode->addStyleRule(rule->clone());
            }
        }
    }
    
    return styleNode;
}

std::shared_ptr<BaseNode> TemplateNode::instantiateElementTemplate(const std::map<std::string, std::string>& args) const {
    auto elementNode = std::make_shared<ElementNode>("div"); // 默认元素
    
    // 处理模板内容
    for (const auto& content : templateContent) {
        if (content->getNodeType() == NodeType::ELEMENT) {
            auto elementContent = std::static_pointer_cast<ElementNode>(content);
            
            // 复制元素属性
            for (const auto& attr : elementContent->getAttributes()) {
                std::string processedValue = processTemplateString(attr.second, args);
                elementNode->setAttribute(attr.first, processedValue);
            }
            
            // 复制子节点
            for (const auto& child : elementContent->getChildren()) {
                elementNode->addChild(child->clone());
            }
        }
    }
    
    return elementNode;
}

std::shared_ptr<BaseNode> TemplateNode::instantiateVariableTemplate(const std::map<std::string, std::string>& args) const {
    auto variableNode = std::make_shared<BaseNode>(NodeType::TEXT, "variable");
    
    // 处理模板内容
    for (const auto& content : templateContent) {
        if (content->getNodeType() == NodeType::TEXT) {
            auto textContent = std::static_pointer_cast<TextNode>(content);
            std::string processedValue = processTemplateString(textContent->getText(), args);
            variableNode->setValue(processedValue);
        }
    }
    
    return variableNode;
}

std::shared_ptr<TemplateNode> TemplateNode::combine(const std::shared_ptr<TemplateNode>& other) const {
    if (!other) {
        return nullptr;
    }
    
    auto combined = std::make_shared<TemplateNode>(templateName + "_" + other->templateName, templateType);
    
    // 合并参数
    for (const auto& param : parameters) {
        combined->setParameter(param.first, param.second);
    }
    for (const auto& param : other->parameters) {
        combined->setParameter(param.first, param.second);
    }
    
    // 合并内容
    for (const auto& content : templateContent) {
        combined->addTemplateContent(content->clone());
    }
    for (const auto& content : other->templateContent) {
        combined->addTemplateContent(content->clone());
    }
    
    // 合并约束
    for (const auto& constraint : constraints) {
        combined->addConstraint(constraint);
    }
    for (const auto& constraint : other->constraints) {
        combined->addConstraint(constraint);
    }
    
    return combined;
}

std::shared_ptr<TemplateNode> TemplateNode::merge(const std::shared_ptr<TemplateNode>& other) const {
    if (!other) {
        return nullptr;
    }
    
    auto merged = std::make_shared<TemplateNode>(templateName, templateType);
    
    // 合并参数（其他模板的参数优先）
    for (const auto& param : parameters) {
        merged->setParameter(param.first, param.second);
    }
    for (const auto& param : other->parameters) {
        merged->setParameter(param.first, param.second);
    }
    
    // 合并内容（其他模板的内容优先）
    for (const auto& content : templateContent) {
        merged->addTemplateContent(content->clone());
    }
    for (const auto& content : other->templateContent) {
        merged->addTemplateContent(content->clone());
    }
    
    return merged;
}

bool TemplateNode::validateTemplate() const {
    // 检查模板名称
    if (templateName.empty()) {
        return false;
    }
    
    // 检查模板内容
    if (templateContent.empty()) {
        return false;
    }
    
    // 检查约束
    return validateConstraints();
}

bool TemplateNode::validateParameters(const std::map<std::string, std::string>& args) const {
    // 检查必需参数
    for (const auto& param : parameters) {
        if (args.find(param.first) == args.end()) {
            return false;
        }
    }
    
    // 检查类型约束
    for (const auto& constraint : typeConstraints) {
        auto it = args.find(constraint.first);
        if (it != args.end()) {
            if (!checkTypeConstraint(constraint.first, it->second)) {
                return false;
            }
        }
    }
    
    return true;
}

bool TemplateNode::validateConstraints() const {
    for (const auto& constraint : constraints) {
        if (!checkGeneralConstraint(constraint)) {
            return false;
        }
    }
    
    return true;
}

std::string TemplateNode::toHTML() const {
    std::ostringstream oss;
    
    oss << "<!-- Template: " << templateName << " -->\n";
    
    for (const auto& content : templateContent) {
        oss << content->toHTML() << "\n";
    }
    
    return oss.str();
}

std::string TemplateNode::toCSS() const {
    if (templateType != TemplateType::STYLE) {
        return "";
    }
    
    std::ostringstream oss;
    
    for (const auto& content : templateContent) {
        if (content->getNodeType() == NodeType::STYLE) {
            auto styleContent = std::static_pointer_cast<StyleNode>(content);
            oss << styleContent->toCSS() << "\n";
        }
    }
    
    return oss.str();
}

std::string TemplateNode::toJavaScript() const {
    std::ostringstream oss;
    
    for (const auto& content : templateContent) {
        if (content->getNodeType() == NodeType::SCRIPT) {
            auto scriptContent = std::static_pointer_cast<ScriptNode>(content);
            oss << scriptContent->toJavaScript() << "\n";
        }
    }
    
    return oss.str();
}

std::string TemplateNode::toTemplateString() const {
    std::ostringstream oss;
    
    oss << "[" << getTemplateTypeName() << "] " << templateName << "\n";
    
    if (!parameters.empty()) {
        oss << "Parameters:\n";
        for (const auto& param : parameters) {
            oss << "  " << param.first << ": " << param.second << "\n";
        }
    }
    
    if (!parentTemplates.empty()) {
        oss << "Parents: " << parentTemplates[0];
        for (size_t i = 1; i < parentTemplates.size(); ++i) {
            oss << ", " << parentTemplates[i];
        }
        oss << "\n";
    }
    
    oss << "Content:\n";
    for (const auto& content : templateContent) {
        oss << "  " << content->toString() << "\n";
    }
    
    return oss.str();
}

std::string TemplateNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << BaseNode::getDebugInfo();
    oss << "TemplateType: " << getTemplateTypeName() << "\n";
    oss << "TemplateName: " << templateName << "\n";
    oss << "Parameters: " << parameters.size() << "\n";
    oss << "Content: " << templateContent.size() << "\n";
    oss << "Parents: " << parentTemplates.size() << "\n";
    oss << "Constraints: " << constraints.size() << "\n";
    oss << "IsAbstract: " << (isAbstract ? "true" : "false") << "\n";
    oss << "IsFinal: " << (isFinal ? "true" : "false") << "\n";
    oss << "Priority: " << priority << "\n";
    return oss.str();
}

std::shared_ptr<BaseNode> TemplateNode::clone() const {
    auto cloned = std::make_shared<TemplateNode>(templateName, templateType);
    cloned->parameters = parameters;
    cloned->parentTemplates = parentTemplates;
    cloned->inheritedProperties = inheritedProperties;
    cloned->constraints = constraints;
    cloned->typeConstraints = typeConstraints;
    cloned->metadata = metadata;
    cloned->isAbstract = isAbstract;
    cloned->isFinal = isFinal;
    cloned->priority = priority;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& content : templateContent) {
        cloned->addTemplateContent(content->clone());
    }
    
    return cloned;
}

bool TemplateNode::operator==(const TemplateNode& other) const {
    return BaseNode::operator==(other) &&
           templateType == other.templateType &&
           templateName == other.templateName &&
           parameters == other.parameters &&
           parentTemplates == other.parentTemplates &&
           constraints == other.constraints &&
           isAbstract == other.isAbstract &&
           isFinal == other.isFinal &&
           priority == other.priority;
}

bool TemplateNode::operator!=(const TemplateNode& other) const {
    return !(*this == other);
}

std::string TemplateNode::processTemplateString(const std::string& templateStr, const std::map<std::string, std::string>& args) const {
    std::string result = templateStr;
    
    // 替换参数占位符
    for (const auto& arg : args) {
        std::string placeholder = "{{" + arg.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), arg.second);
            pos += arg.second.length();
        }
    }
    
    return result;
}

std::string TemplateNode::replacePlaceholders(const std::string& str, const std::map<std::string, std::string>& replacements) const {
    std::string result = str;
    
    for (const auto& replacement : replacements) {
        std::string placeholder = "{{" + replacement.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), replacement.second);
            pos += replacement.second.length();
        }
    }
    
    return result;
}

std::vector<std::string> TemplateNode::parseTemplateString(const std::string& templateStr) const {
    std::vector<std::string> parts;
    std::regex placeholderRegex(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator begin(templateStr.begin(), templateStr.end(), placeholderRegex);
    std::sregex_iterator end;
    
    size_t lastPos = 0;
    for (auto it = begin; it != end; ++it) {
        size_t pos = it->position();
        if (pos > lastPos) {
            parts.push_back(templateStr.substr(lastPos, pos - lastPos));
        }
        parts.push_back(it->str(1));
        lastPos = pos + it->length();
    }
    
    if (lastPos < templateStr.length()) {
        parts.push_back(templateStr.substr(lastPos));
    }
    
    return parts;
}

std::string TemplateNode::parseStyleTemplate(const std::string& templateStr) const {
    // 简化的样式模板解析
    return templateStr;
}

std::string TemplateNode::parseElementTemplate(const std::string& templateStr) const {
    // 简化的元素模板解析
    return templateStr;
}

std::string TemplateNode::parseVariableTemplate(const std::string& templateStr) const {
    // 简化的变量模板解析
    return templateStr;
}

void TemplateNode::processInheritance() {
    // 处理模板继承
    mergeParentProperties();
    resolveInheritanceConflicts();
}

void TemplateNode::mergeParentProperties() {
    // 合并父模板属性
    for (const auto& parent : parentTemplates) {
        // 这里需要从模板注册表中获取父模板
        // 简化实现
    }
}

void TemplateNode::resolveInheritanceConflicts() {
    // 解决继承冲突
    // 简化实现
}

bool TemplateNode::checkTypeConstraint(const std::string& type, const std::string& value) const {
    // 简化的类型约束检查
    if (type == "number") {
        try {
            std::stod(value);
            return true;
        } catch (...) {
            return false;
        }
    } else if (type == "string") {
        return true;
    } else if (type == "boolean") {
        return value == "true" || value == "false";
    }
    
    return true;
}

bool TemplateNode::checkGeneralConstraint(const std::string& constraint) const {
    // 简化的通用约束检查
    return true;
}

} // namespace CHTL