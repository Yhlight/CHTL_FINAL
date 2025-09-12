#include "CHTLTemplateProcessor.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLTemplateProcessor::CHTLTemplateProcessor() {}

void CHTLTemplateProcessor::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLTemplateProcessor::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

std::shared_ptr<TemplateNode> CHTLTemplateProcessor::getTemplate(const std::string& name) const {
    auto it = templates.find(name);
    if (it != templates.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLTemplateProcessor::mergeTemplateProperties(std::shared_ptr<TemplateNode> target, std::shared_ptr<TemplateNode> source) {
    for (const auto& prop : source->getAttributes()) {
        if (!target->hasAttribute(prop.first)) {
            target->setAttribute(prop.first, prop.second);
        }
    }
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

void CHTLTemplateProcessor::mergeTemplateContent(std::shared_ptr<TemplateNode> target, std::shared_ptr<TemplateNode> source) {
    for (const auto& content : source->getTemplateContent()) {
        target->addTemplateContent(content->clone());
    }
}

void CHTLTemplateProcessor::registerTemplate(std::shared_ptr<TemplateNode> templateNode) {
    if (templateNode) {
        templates[templateNode->getTemplateName()] = templateNode;
    }
}

void CHTLTemplateProcessor::unregisterTemplate(const std::string& name) {
    templates.erase(name);
}

bool CHTLTemplateProcessor::hasTemplate(const std::string& name) const {
    return templates.count(name) > 0;
}

std::shared_ptr<BaseNode> CHTLTemplateProcessor::processTemplate(const std::string& name, const std::map<std::string, std::string>& args) {
    auto templateNode = getTemplate(name);
    if (!templateNode) {
        addError("模板 '" + name + "' 未找到");
        return nullptr;
    }

    // 应用继承
    applyInheritance(templateNode);

    // 实例化模板
    return templateNode->instantiate(args);
}

std::shared_ptr<BaseNode> CHTLTemplateProcessor::instantiateTemplate(const std::string& name, const std::map<std::string, std::string>& args) {
    auto templateNode = getTemplate(name);
    if (!templateNode) {
        addError("模板 '" + name + "' 未找到");
        return nullptr;
    }
    return templateNode->instantiate(args);
}

std::shared_ptr<TemplateNode> CHTLTemplateProcessor::combineTemplates(const std::string& template1Name, const std::string& template2Name) {
    auto template1 = getTemplate(template1Name);
    auto template2 = getTemplate(template2Name);

    if (!template1 || !template2) {
        addError("组合模板时，一个或多个模板未找到");
        return nullptr;
    }

    return template1->combine(template2);
}

std::shared_ptr<TemplateNode> CHTLTemplateProcessor::mergeTemplates(const std::string& template1Name, const std::string& template2Name) {
    auto template1 = getTemplate(template1Name);
    auto template2 = getTemplate(template2Name);

    if (!template1 || !template2) {
        addError("合并模板时，一个或多个模板未找到");
        return nullptr;
    }

    return template1->merge(template2);
}

void CHTLTemplateProcessor::applyInheritance(std::shared_ptr<TemplateNode> templateNode) {
    if (!templateNode) return;

    for (const auto& parentName : templateNode->getParentTemplates()) {
        auto parentTemplate = getTemplate(parentName);
        if (parentTemplate) {
            // 递归应用父模板的继承
            applyInheritance(parentTemplate);
            // 合并父模板的属性和内容
            mergeTemplateProperties(templateNode, parentTemplate);
            mergeTemplateContent(templateNode, parentTemplate);
        } else {
            addWarning("父模板 '" + parentName + "' 未找到，无法应用继承");
        }
    }
    resolveInheritanceConflicts(templateNode);
}

void CHTLTemplateProcessor::resolveInheritanceConflicts(std::shared_ptr<TemplateNode> templateNode) {
    // 简单的冲突解决策略：子模板的属性优先
    // 对于内容，目前是简单追加，更复杂的策略可能需要用户定义
    // 可以在这里添加更复杂的逻辑，例如根据优先级或特定规则覆盖
}

const std::vector<std::string>& CHTLTemplateProcessor::getErrors() const {
    return errors;
}

const std::vector<std::string>& CHTLTemplateProcessor::getWarnings() const {
    return warnings;
}

void CHTLTemplateProcessor::clearMessages() {
    errors.clear();
    warnings.clear();
}

void CHTLTemplateProcessor::reset() {
    templates.clear();
    clearMessages();
}

std::string CHTLTemplateProcessor::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTL模板处理器调试信息:\n";
    oss << "注册模板数: " << templates.size() << "\n";
    oss << "错误数: " << errors.size() << "\n";
    oss << "警告数: " << warnings.size() << "\n";
    return oss.str();
}

} // namespace CHTL