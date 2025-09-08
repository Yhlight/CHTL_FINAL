#include "CHTLContext.h"

namespace CHTL {

const std::string CHTLContext::GLOBAL_NAMESPACE = "_global";

void CHTLContext::registerTemplate(const std::string& ns, std::unique_ptr<TemplateDefinitionNode> templateNode) {
    if (templateNode) {
        m_templates[ns][templateNode->getTemplateName()] = std::move(templateNode);
    }
}

const TemplateDefinitionNode* CHTLContext::getTemplate(const std::string& ns, const std::string& name) const {
    auto ns_it = m_templates.find(ns);
    if (ns_it != m_templates.end()) {
        const auto& registry = ns_it->second;
        auto item_it = registry.find(name);
        if (item_it != registry.end()) {
            return item_it->second.get();
        }
    }
    return nullptr;
}

void CHTLContext::registerOrigin(const std::string& ns, std::unique_ptr<OriginNode> originNode) {
    if (originNode && !originNode->getName().empty()) {
        m_origins[ns][originNode->getName()] = std::move(originNode);
    }
}

const OriginNode* CHTLContext::getOrigin(const std::string& ns, const std::string& name) const {
    auto ns_it = m_origins.find(ns);
    if (ns_it != m_origins.end()) {
        const auto& registry = ns_it->second;
        auto item_it = registry.find(name);
        if (item_it != registry.end()) {
            return item_it->second.get();
        }
    }
    return nullptr;
}

void CHTLContext::registerVariableGroup(const std::string& ns, std::unique_ptr<VariableGroupDefinitionNode> varGroupNode) {
    if (varGroupNode) {
        m_variable_groups[ns][varGroupNode->getName()] = std::move(varGroupNode);
    }
}

const VariableGroupDefinitionNode* CHTLContext::getVariableGroup(const std::string& ns, const std::string& name) const {
    auto ns_it = m_variable_groups.find(ns);
    if (ns_it != m_variable_groups.end()) {
        const auto& registry = ns_it->second;
        auto item_it = registry.find(name);
        if (item_it != registry.end()) {
            return item_it->second.get();
        }
    }
    return nullptr;
}

void CHTLContext::registerCustom(const std::string& ns, std::unique_ptr<CustomDefinitionNode> customNode) {
    if (customNode) {
        m_customs[ns][customNode->getCustomName()] = std::move(customNode);
    }
}

const CustomDefinitionNode* CHTLContext::getCustom(const std::string& ns, const std::string& name) const {
    auto it = m_customs.find(ns);
    if (it != m_customs.end()) {
        const auto& registry = it->second;
        auto item_it = registry.find(name);
        if (item_it != registry.end()) {
            return item_it->second.get();
        }
    }
    return nullptr;
}

} // namespace CHTL
