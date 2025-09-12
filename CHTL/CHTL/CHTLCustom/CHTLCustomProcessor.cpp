#include "CHTLCustomProcessor.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLCustomProcessor::CHTLCustomProcessor() {}

void CHTLCustomProcessor::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLCustomProcessor::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

std::shared_ptr<CustomNode> CHTLCustomProcessor::getCustomNode(const std::string& name) const {
    auto it = customNodes.find(name);
    if (it != customNodes.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLCustomProcessor::mergeCustomProperties(std::shared_ptr<CustomNode> target, std::shared_ptr<CustomNode> source) {
    for (const auto& param : source->getParameters()) {
        if (!target->hasParameter(param.first)) {
            target->setParameter(param.first, param.second);
        }
    }
    for (const auto& meta : source->getMetadata()) {
        if (!target->hasMetadata(meta.first)) {
            target->setMetadata(meta.first, meta.second);
        }
    }
}

void CHTLCustomProcessor::mergeCustomContent(std::shared_ptr<CustomNode> target, std::shared_ptr<CustomNode> source) {
    for (const auto& content : source->getCustomContent()) {
        target->addCustomContent(content->clone());
    }
}

void CHTLCustomProcessor::registerCustomNode(std::shared_ptr<CustomNode> customNode) {
    if (customNode) {
        customNodes[customNode->getCustomName()] = customNode;
    }
}

void CHTLCustomProcessor::unregisterCustomNode(const std::string& name) {
    customNodes.erase(name);
}

bool CHTLCustomProcessor::hasCustomNode(const std::string& name) const {
    return customNodes.count(name) > 0;
}

std::shared_ptr<BaseNode> CHTLCustomProcessor::processCustomNode(const std::string& name, const std::map<std::string, std::string>& args) {
    auto customNode = getCustomNode(name);
    if (!customNode) {
        addError("自定义节点 '" + name + "' 未找到");
        return nullptr;
    }

    // 应用特例化
    applySpecializations(customNode);

    // 实例化自定义节点
    return customNode->instantiate(args);
}

std::shared_ptr<BaseNode> CHTLCustomProcessor::instantiateCustomNode(const std::string& name, const std::map<std::string, std::string>& args) {
    auto customNode = getCustomNode(name);
    if (!customNode) {
        addError("自定义节点 '" + name + "' 未找到");
        return nullptr;
    }
    return customNode->instantiate(args);
}

std::shared_ptr<CustomNode> CHTLCustomProcessor::combineCustomNodes(const std::string& custom1Name, const std::string& custom2Name) {
    auto custom1 = getCustomNode(custom1Name);
    auto custom2 = getCustomNode(custom2Name);

    if (!custom1 || !custom2) {
        addError("组合自定义节点时，一个或多个自定义节点未找到");
        return nullptr;
    }

    return custom1->combine(custom2);
}

std::shared_ptr<CustomNode> CHTLCustomProcessor::mergeCustomNodes(const std::string& custom1Name, const std::string& custom2Name) {
    auto custom1 = getCustomNode(custom1Name);
    auto custom2 = getCustomNode(custom2Name);

    if (!custom1 || !custom2) {
        addError("合并自定义节点时，一个或多个自定义节点未找到");
        return nullptr;
    }

    return custom1->merge(custom2);
}

void CHTLCustomProcessor::applySpecializations(std::shared_ptr<CustomNode> customNode) {
    if (!customNode) return;

    // 处理特例化 - 简化实现
    // customNode->processSpecializations(); // 这是私有方法，暂时注释
}

void CHTLCustomProcessor::resolveSpecializationConflicts(std::shared_ptr<CustomNode> customNode) {
    if (!customNode) return;

    // 解决特例化冲突
    // 简化实现：子自定义的特例化优先
}

void CHTLCustomProcessor::addSpecialization(const std::string& customName, const std::string& key, std::shared_ptr<BaseNode> specialization) {
    auto customNode = getCustomNode(customName);
    if (customNode) {
        customNode->addSpecialization(key, specialization);
    } else {
        addError("自定义节点 '" + customName + "' 未找到，无法添加特例化");
    }
}

void CHTLCustomProcessor::removeSpecialization(const std::string& customName, const std::string& key) {
    auto customNode = getCustomNode(customName);
    if (customNode) {
        customNode->removeSpecialization(key);
    } else {
        addError("自定义节点 '" + customName + "' 未找到，无法删除特例化");
    }
}

std::shared_ptr<BaseNode> CHTLCustomProcessor::getSpecialization(const std::string& customName, const std::string& key) const {
    auto customNode = getCustomNode(customName);
    if (customNode) {
        return customNode->getSpecialization(key);
    }
    return nullptr;
}

void CHTLCustomProcessor::deleteElement(const std::string& customName, const std::string& elementName) {
    auto customNode = getCustomNode(customName);
    if (customNode) {
        // 查找并删除指定名称的元素
        auto& content = customNode->getCustomContent();
        for (auto it = content.begin(); it != content.end(); ++it) {
            if ((*it)->toString() == elementName) {
                customNode->removeElement(*it);
                break;
            }
        }
    } else {
        addError("自定义节点 '" + customName + "' 未找到，无法删除元素");
    }
}

void CHTLCustomProcessor::deleteElementAt(const std::string& customName, int index) {
    auto customNode = getCustomNode(customName);
    if (customNode) {
        customNode->removeElementAt(index);
    } else {
        addError("自定义节点 '" + customName + "' 未找到，无法删除元素");
    }
}

void CHTLCustomProcessor::deleteSpecialization(const std::string& customName, const std::string& key) {
    removeSpecialization(customName, key);
}

void CHTLCustomProcessor::insertElement(const std::string& customName, std::shared_ptr<BaseNode> element) {
    auto customNode = getCustomNode(customName);
    if (customNode) {
        customNode->insertElement(element);
    } else {
        addError("自定义节点 '" + customName + "' 未找到，无法插入元素");
    }
}

void CHTLCustomProcessor::insertElementAt(const std::string& customName, int index, std::shared_ptr<BaseNode> element) {
    auto customNode = getCustomNode(customName);
    if (customNode) {
        customNode->insertElementAt(index, element);
    } else {
        addError("自定义节点 '" + customName + "' 未找到，无法插入元素");
    }
}

void CHTLCustomProcessor::insertSpecialization(const std::string& customName, const std::string& key, std::shared_ptr<BaseNode> specialization) {
    addSpecialization(customName, key, specialization);
}

std::shared_ptr<BaseNode> CHTLCustomProcessor::getIndexedElement(const std::string& customName, int index) const {
    auto customNode = getCustomNode(customName);
    if (customNode) {
        return customNode->getIndexedContent(index);
    }
    return nullptr;
}

void CHTLCustomProcessor::setIndexedElement(const std::string& customName, int index, std::shared_ptr<BaseNode> element) {
    auto customNode = getCustomNode(customName);
    if (customNode) {
        customNode->setIndexedContent(index, element);
    } else {
        addError("自定义节点 '" + customName + "' 未找到，无法设置索引元素");
    }
}

void CHTLCustomProcessor::removeIndexedElement(const std::string& customName, int index) {
    auto customNode = getCustomNode(customName);
    if (customNode) {
        customNode->removeIndexedContent(index);
    } else {
        addError("自定义节点 '" + customName + "' 未找到，无法删除索引元素");
    }
}

bool CHTLCustomProcessor::validateCustomNode(const std::string& name) const {
    auto customNode = getCustomNode(name);
    if (!customNode) {
        return false;
    }
    return customNode->validateCustom();
}

bool CHTLCustomProcessor::validateCustomParameters(const std::string& name, const std::map<std::string, std::string>& args) const {
    auto customNode = getCustomNode(name);
    if (!customNode) {
        return false;
    }
    return customNode->validateParameters(args);
}

bool CHTLCustomProcessor::validateCustomConstraints(const std::string& name) const {
    auto customNode = getCustomNode(name);
    if (!customNode) {
        return false;
    }
    return customNode->validateConstraints();
}

const std::vector<std::string>& CHTLCustomProcessor::getErrors() const {
    return errors;
}

const std::vector<std::string>& CHTLCustomProcessor::getWarnings() const {
    return warnings;
}

void CHTLCustomProcessor::clearMessages() {
    errors.clear();
    warnings.clear();
}

void CHTLCustomProcessor::reset() {
    customNodes.clear();
    clearMessages();
}

std::string CHTLCustomProcessor::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTL自定义处理器调试信息:\n";
    oss << "注册自定义节点数: " << customNodes.size() << "\n";
    oss << "错误数: " << errors.size() << "\n";
    oss << "警告数: " << warnings.size() << "\n";
    return oss.str();
}

} // namespace CHTL