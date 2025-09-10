#include "CustomNode.h"
#include "BaseNode.h"
#include "ElementNode.h"
#include "TextNode.h"
#include "CommentNode.h"
#include "StyleNode.h"
#include "ScriptNode.h"
#include "TemplateNode.h"
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

CustomNode::CustomNode(const std::string& name, CustomType type)
    : BaseNode(NodeType::CUSTOM, name), customType(type), customName(name), 
      isAbstract(false), isFinal(false), priority(0) {
}

std::string CustomNode::getCustomTypeName() const {
    switch (customType) {
        case CustomType::STYLE: return "Style";
        case CustomType::ELEMENT: return "Element";
        case CustomType::VARIABLE: return "Variable";
        case CustomType::FUNCTION: return "Function";
        case CustomType::MIXED: return "Mixed";
        default: return "Unknown";
    }
}

void CustomNode::setParameter(const std::string& name, const std::string& value) {
    parameters[name] = value;
}

std::string CustomNode::getParameter(const std::string& name) const {
    auto it = parameters.find(name);
    return (it != parameters.end()) ? it->second : "";
}

bool CustomNode::hasParameter(const std::string& name) const {
    return parameters.find(name) != parameters.end();
}

void CustomNode::removeParameter(const std::string& name) {
    parameters.erase(name);
}

void CustomNode::addCustomContent(std::shared_ptr<BaseNode> content) {
    if (content) {
        customContent.push_back(content);
    }
}

void CustomNode::removeCustomContent(std::shared_ptr<BaseNode> content) {
    auto it = std::find(customContent.begin(), customContent.end(), content);
    if (it != customContent.end()) {
        customContent.erase(it);
    }
}

void CustomNode::addSpecialization(const std::string& key, std::shared_ptr<BaseNode> specialization) {
    if (specialization) {
        specializations[key] = specialization;
        if (std::find(specializationKeys.begin(), specializationKeys.end(), key) == specializationKeys.end()) {
            specializationKeys.push_back(key);
        }
    }
}

void CustomNode::removeSpecialization(const std::string& key) {
    specializations.erase(key);
    auto it = std::find(specializationKeys.begin(), specializationKeys.end(), key);
    if (it != specializationKeys.end()) {
        specializationKeys.erase(it);
    }
}

std::shared_ptr<BaseNode> CustomNode::getSpecialization(const std::string& key) const {
    auto it = specializations.find(key);
    return (it != specializations.end()) ? it->second : nullptr;
}

bool CustomNode::hasSpecialization(const std::string& key) const {
    return specializations.find(key) != specializations.end();
}

void CustomNode::setIndexedContent(int index, std::shared_ptr<BaseNode> content) {
    if (content) {
        indexedContent[index] = content;
        updateIndexKeys();
    }
}

void CustomNode::removeIndexedContent(int index) {
    indexedContent.erase(index);
    updateIndexKeys();
}

std::shared_ptr<BaseNode> CustomNode::getIndexedContent(int index) const {
    auto it = indexedContent.find(index);
    return (it != indexedContent.end()) ? it->second : nullptr;
}

bool CustomNode::hasIndexedContent(int index) const {
    return indexedContent.find(index) != indexedContent.end();
}

void CustomNode::insertElement(std::shared_ptr<BaseNode> element) {
    if (element) {
        insertedElements.push_back(element);
        updateElementLists();
    }
}

void CustomNode::insertElementAt(int index, std::shared_ptr<BaseNode> element) {
    if (element && index >= 0 && index <= static_cast<int>(insertedElements.size())) {
        insertedElements.insert(insertedElements.begin() + index, element);
        updateElementLists();
    }
}

void CustomNode::removeElement(std::shared_ptr<BaseNode> element) {
    auto it = std::find(insertedElements.begin(), insertedElements.end(), element);
    if (it != insertedElements.end()) {
        insertedElements.erase(it);
        deletedElements.push_back(element);
        updateElementLists();
    }
}

void CustomNode::removeElementAt(int index) {
    if (index >= 0 && index < static_cast<int>(insertedElements.size())) {
        auto element = insertedElements[index];
        insertedElements.erase(insertedElements.begin() + index);
        deletedElements.push_back(element);
        updateElementLists();
    }
}

void CustomNode::clearElements() {
    insertedElements.clear();
    deletedElements.clear();
    updateElementLists();
}

void CustomNode::addConstraint(const std::string& constraint) {
    if (std::find(constraints.begin(), constraints.end(), constraint) == constraints.end()) {
        constraints.push_back(constraint);
    }
}

void CustomNode::removeConstraint(const std::string& constraint) {
    auto it = std::find(constraints.begin(), constraints.end(), constraint);
    if (it != constraints.end()) {
        constraints.erase(it);
    }
}

bool CustomNode::hasConstraint(const std::string& constraint) const {
    return std::find(constraints.begin(), constraints.end(), constraint) != constraints.end();
}

void CustomNode::setTypeConstraint(const std::string& type, const std::string& constraint) {
    typeConstraints[type] = constraint;
}

std::string CustomNode::getTypeConstraint(const std::string& type) const {
    auto it = typeConstraints.find(type);
    return (it != typeConstraints.end()) ? it->second : "";
}

bool CustomNode::hasTypeConstraint(const std::string& type) const {
    return typeConstraints.find(type) != typeConstraints.end();
}

void CustomNode::removeTypeConstraint(const std::string& type) {
    typeConstraints.erase(type);
}

void CustomNode::setValidationFunction(const std::string& type, std::function<bool(const std::string&)> validator) {
    validationFunctions[type] = validator;
}

std::function<bool(const std::string&)> CustomNode::getValidationFunction(const std::string& type) const {
    auto it = validationFunctions.find(type);
    return (it != validationFunctions.end()) ? it->second : nullptr;
}

bool CustomNode::hasValidationFunction(const std::string& type) const {
    return validationFunctions.find(type) != validationFunctions.end();
}

void CustomNode::removeValidationFunction(const std::string& type) {
    validationFunctions.erase(type);
}

void CustomNode::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string CustomNode::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    return (it != metadata.end()) ? it->second : "";
}

bool CustomNode::hasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

void CustomNode::removeMetadata(const std::string& key) {
    metadata.erase(key);
}

void CustomNode::setCustomMethod(const std::string& name, std::function<std::string(const std::vector<std::string>&)> method) {
    customMethods[name] = method;
}

std::string CustomNode::callCustomMethod(const std::string& name, const std::vector<std::string>& args) const {
    auto it = customMethods.find(name);
    if (it != customMethods.end()) {
        return it->second(args);
    }
    return "";
}

bool CustomNode::hasCustomMethod(const std::string& name) const {
    return customMethods.find(name) != customMethods.end();
}

void CustomNode::removeCustomMethod(const std::string& name) {
    customMethods.erase(name);
}

void CustomNode::setCustomConstructor(const std::string& name, std::function<std::shared_ptr<BaseNode>(const std::map<std::string, std::string>&)> constructor) {
    customConstructors[name] = constructor;
}

std::shared_ptr<BaseNode> CustomNode::callCustomConstructor(const std::string& name, const std::map<std::string, std::string>& args) const {
    auto it = customConstructors.find(name);
    if (it != customConstructors.end()) {
        return it->second(args);
    }
    return nullptr;
}

bool CustomNode::hasCustomConstructor(const std::string& name) const {
    return customConstructors.find(name) != customConstructors.end();
}

void CustomNode::removeCustomConstructor(const std::string& name) {
    customConstructors.erase(name);
}

std::shared_ptr<BaseNode> CustomNode::instantiate(const std::map<std::string, std::string>& args) const {
    switch (customType) {
        case CustomType::STYLE:
            return instantiateStyleCustom(args);
        case CustomType::ELEMENT:
            return instantiateElementCustom(args);
        case CustomType::VARIABLE:
            return instantiateVariableCustom(args);
        case CustomType::FUNCTION:
            return instantiateFunctionCustom(args);
        case CustomType::MIXED:
            return instantiateElementCustom(args);
        default:
            return nullptr;
    }
}

std::shared_ptr<BaseNode> CustomNode::instantiateStyleCustom(const std::map<std::string, std::string>& args) const {
    auto styleNode = std::make_shared<StyleNode>(false); // 全局样式
    
    // 处理自定义内容
    for (const auto& content : customContent) {
        if (content->getNodeType() == NodeType::STYLE) {
            auto styleContent = std::static_pointer_cast<StyleNode>(content);
            
            // 复制样式属性
            for (const auto& prop : styleContent->getStyleProperties()) {
                std::string processedValue = processCustomString(prop.second, args);
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

std::shared_ptr<BaseNode> CustomNode::instantiateElementCustom(const std::map<std::string, std::string>& args) const {
    auto elementNode = std::make_shared<ElementNode>("div"); // 默认元素
    
    // 处理自定义内容
    for (const auto& content : customContent) {
        if (content->getNodeType() == NodeType::ELEMENT) {
            auto elementContent = std::static_pointer_cast<ElementNode>(content);
            
            // 复制元素属性
            for (const auto& attr : elementContent->getAttributes()) {
                std::string processedValue = processCustomString(attr.second, args);
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

std::shared_ptr<BaseNode> CustomNode::instantiateVariableCustom(const std::map<std::string, std::string>& args) const {
    auto variableNode = std::make_shared<BaseNode>(NodeType::TEXT, "variable");
    
    // 处理自定义内容
    for (const auto& content : customContent) {
        if (content->getNodeType() == NodeType::TEXT) {
            auto textContent = std::static_pointer_cast<TextNode>(content);
            std::string processedValue = processCustomString(textContent->getText(), args);
            variableNode->setValue(processedValue);
        }
    }
    
    return variableNode;
}

std::shared_ptr<BaseNode> CustomNode::instantiateFunctionCustom(const std::map<std::string, std::string>& args) const {
    auto functionNode = std::make_shared<BaseNode>(NodeType::SCRIPT, "function");
    
    // 处理自定义内容
    for (const auto& content : customContent) {
        if (content->getNodeType() == NodeType::SCRIPT) {
            auto scriptContent = std::static_pointer_cast<ScriptNode>(content);
            std::string processedValue = processCustomString(scriptContent->getScriptContent(), args);
            functionNode->setValue(processedValue);
        }
    }
    
    return functionNode;
}

std::shared_ptr<CustomNode> CustomNode::combine(const std::shared_ptr<CustomNode>& other) const {
    if (!other) {
        return nullptr;
    }
    
    auto combined = std::make_shared<CustomNode>(customName + "_" + other->customName, customType);
    
    // 合并参数
    for (const auto& param : parameters) {
        combined->setParameter(param.first, param.second);
    }
    for (const auto& param : other->parameters) {
        combined->setParameter(param.first, param.second);
    }
    
    // 合并内容
    for (const auto& content : customContent) {
        combined->addCustomContent(content->clone());
    }
    for (const auto& content : other->customContent) {
        combined->addCustomContent(content->clone());
    }
    
    // 合并特例化
    for (const auto& spec : specializations) {
        combined->addSpecialization(spec.first, spec.second->clone());
    }
    for (const auto& spec : other->specializations) {
        combined->addSpecialization(spec.first, spec.second->clone());
    }
    
    return combined;
}

std::shared_ptr<CustomNode> CustomNode::merge(const std::shared_ptr<CustomNode>& other) const {
    if (!other) {
        return nullptr;
    }
    
    auto merged = std::make_shared<CustomNode>(customName, customType);
    
    // 合并参数（其他自定义的参数优先）
    for (const auto& param : parameters) {
        merged->setParameter(param.first, param.second);
    }
    for (const auto& param : other->parameters) {
        merged->setParameter(param.first, param.second);
    }
    
    // 合并内容（其他自定义的内容优先）
    for (const auto& content : customContent) {
        merged->addCustomContent(content->clone());
    }
    for (const auto& content : other->customContent) {
        merged->addCustomContent(content->clone());
    }
    
    return merged;
}

bool CustomNode::validateCustom() const {
    // 检查自定义名称
    if (customName.empty()) {
        return false;
    }
    
    // 检查自定义内容
    if (customContent.empty()) {
        return false;
    }
    
    // 检查约束
    return validateConstraints();
}

bool CustomNode::validateParameters(const std::map<std::string, std::string>& args) const {
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
    
    // 检查验证函数
    for (const auto& validator : validationFunctions) {
        auto it = args.find(validator.first);
        if (it != args.end()) {
            if (!checkValidationFunction(validator.first, it->second)) {
                return false;
            }
        }
    }
    
    return true;
}

bool CustomNode::validateConstraints() const {
    for (const auto& constraint : constraints) {
        if (!checkGeneralConstraint(constraint)) {
            return false;
        }
    }
    
    return true;
}

bool CustomNode::validateSpecializations() const {
    for (const auto& spec : specializations) {
        if (!spec.second) {
            return false;
        }
    }
    
    return true;
}

std::string CustomNode::toHTML() const {
    std::ostringstream oss;
    
    oss << "<!-- Custom: " << customName << " -->\n";
    
    for (const auto& content : customContent) {
        oss << content->toHTML() << "\n";
    }
    
    return oss.str();
}

std::string CustomNode::toCSS() const {
    if (customType != CustomType::STYLE) {
        return "";
    }
    
    std::ostringstream oss;
    
    for (const auto& content : customContent) {
        if (content->getNodeType() == NodeType::STYLE) {
            auto styleContent = std::static_pointer_cast<StyleNode>(content);
            oss << styleContent->toCSS() << "\n";
        }
    }
    
    return oss.str();
}

std::string CustomNode::toJavaScript() const {
    std::ostringstream oss;
    
    for (const auto& content : customContent) {
        if (content->getNodeType() == NodeType::SCRIPT) {
            auto scriptContent = std::static_pointer_cast<ScriptNode>(content);
            oss << scriptContent->toJavaScript() << "\n";
        }
    }
    
    return oss.str();
}

std::string CustomNode::toCustomString() const {
    std::ostringstream oss;
    
    oss << "[" << getCustomTypeName() << "] " << customName << "\n";
    
    if (!parameters.empty()) {
        oss << "Parameters:\n";
        for (const auto& param : parameters) {
            oss << "  " << param.first << ": " << param.second << "\n";
        }
    }
    
    if (!specializations.empty()) {
        oss << "Specializations:\n";
        for (const auto& spec : specializations) {
            oss << "  " << spec.first << ": " << spec.second->toString() << "\n";
        }
    }
    
    oss << "Content:\n";
    for (const auto& content : customContent) {
        oss << "  " << content->toString() << "\n";
    }
    
    return oss.str();
}

std::string CustomNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << BaseNode::getDebugInfo();
    oss << "CustomType: " << getCustomTypeName() << "\n";
    oss << "CustomName: " << customName << "\n";
    oss << "Parameters: " << parameters.size() << "\n";
    oss << "Content: " << customContent.size() << "\n";
    oss << "Specializations: " << specializations.size() << "\n";
    oss << "IndexedContent: " << indexedContent.size() << "\n";
    oss << "InsertedElements: " << insertedElements.size() << "\n";
    oss << "DeletedElements: " << deletedElements.size() << "\n";
    oss << "Constraints: " << constraints.size() << "\n";
    oss << "IsAbstract: " << (isAbstract ? "true" : "false") << "\n";
    oss << "IsFinal: " << (isFinal ? "true" : "false") << "\n";
    oss << "Priority: " << priority << "\n";
    return oss.str();
}

std::shared_ptr<BaseNode> CustomNode::clone() const {
    auto cloned = std::make_shared<CustomNode>(customName, customType);
    cloned->parameters = parameters;
    cloned->specializations = specializations;
    cloned->specializationKeys = specializationKeys;
    cloned->indexedContent = indexedContent;
    cloned->indexKeys = indexKeys;
    cloned->insertedElements = insertedElements;
    cloned->deletedElements = deletedElements;
    cloned->constraints = constraints;
    cloned->typeConstraints = typeConstraints;
    cloned->validationFunctions = validationFunctions;
    cloned->metadata = metadata;
    cloned->customMethods = customMethods;
    cloned->customConstructors = customConstructors;
    cloned->isAbstract = isAbstract;
    cloned->isFinal = isFinal;
    cloned->priority = priority;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& content : customContent) {
        cloned->addCustomContent(content->clone());
    }
    
    return cloned;
}

bool CustomNode::operator==(const CustomNode& other) const {
    return BaseNode::operator==(other) &&
           customType == other.customType &&
           customName == other.customName &&
           parameters == other.parameters &&
           specializations.size() == other.specializations.size() &&
           constraints == other.constraints &&
           isAbstract == other.isAbstract &&
           isFinal == other.isFinal &&
           priority == other.priority;
}

bool CustomNode::operator!=(const CustomNode& other) const {
    return !(*this == other);
}

std::string CustomNode::processCustomString(const std::string& customStr, const std::map<std::string, std::string>& args) const {
    std::string result = customStr;
    
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

std::string CustomNode::replacePlaceholders(const std::string& str, const std::map<std::string, std::string>& replacements) const {
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

std::vector<std::string> CustomNode::parseCustomString(const std::string& customStr) const {
    std::vector<std::string> parts;
    std::regex placeholderRegex(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator begin(customStr.begin(), customStr.end(), placeholderRegex);
    std::sregex_iterator end;
    
    size_t lastPos = 0;
    for (auto it = begin; it != end; ++it) {
        size_t pos = it->position();
        if (pos > lastPos) {
            parts.push_back(customStr.substr(lastPos, pos - lastPos));
        }
        parts.push_back(it->str(1));
        lastPos = pos + it->length();
    }
    
    if (lastPos < customStr.length()) {
        parts.push_back(customStr.substr(lastPos));
    }
    
    return parts;
}

std::string CustomNode::parseStyleCustom(const std::string& customStr) const {
    // 简化的样式自定义解析
    return customStr;
}

std::string CustomNode::parseElementCustom(const std::string& customStr) const {
    // 简化的元素自定义解析
    return customStr;
}

std::string CustomNode::parseVariableCustom(const std::string& customStr) const {
    // 简化的变量自定义解析
    return customStr;
}

std::string CustomNode::parseFunctionCustom(const std::string& customStr) const {
    // 简化的函数自定义解析
    return customStr;
}

void CustomNode::processSpecializations() {
    // 处理特例化
    mergeSpecializations();
    resolveSpecializationConflicts();
}

void CustomNode::mergeSpecializations() {
    // 合并特例化
    // 简化实现
}

void CustomNode::resolveSpecializationConflicts() {
    // 解决特例化冲突
    // 简化实现
}

bool CustomNode::checkTypeConstraint(const std::string& type, const std::string& value) const {
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

bool CustomNode::checkGeneralConstraint(const std::string& constraint) const {
    // 简化的通用约束检查
    return true;
}

bool CustomNode::checkValidationFunction(const std::string& type, const std::string& value) const {
    auto it = validationFunctions.find(type);
    if (it != validationFunctions.end()) {
        return it->second(value);
    }
    return true;
}

void CustomNode::updateIndexKeys() {
    indexKeys.clear();
    for (const auto& pair : indexedContent) {
        indexKeys.push_back(pair.first);
    }
    sortIndexKeys();
}

void CustomNode::sortIndexKeys() {
    std::sort(indexKeys.begin(), indexKeys.end());
}

void CustomNode::processInsertions() {
    // 处理插入操作
    updateElementLists();
}

void CustomNode::processDeletions() {
    // 处理删除操作
    updateElementLists();
}

void CustomNode::updateElementLists() {
    // 更新元素列表
    // 简化实现
}

} // namespace CHTL