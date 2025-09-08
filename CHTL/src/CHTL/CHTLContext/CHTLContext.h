#pragma once

#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"
#include <string>
#include <map>
#include <memory>

namespace CHTL {

// Using the correct, memory-safe unique_ptr implementation.
template<typename T>
using ItemRegistry = std::map<std::string, std::unique_ptr<T>>;
template<typename T>
using NamespacedRegistry = std::map<std::string, ItemRegistry<T>>;

class CHTLContext {
public:
    CHTLContext() = default;
    static const std::string GLOBAL_NAMESPACE;

    void registerTemplate(const std::string& ns, std::unique_ptr<TemplateDefinitionNode> templateNode);
    const TemplateDefinitionNode* getTemplate(const std::string& ns, const std::string& name) const;

    void registerCustom(const std::string& ns, std::unique_ptr<CustomDefinitionNode> customNode);
    const CustomDefinitionNode* getCustom(const std::string& ns, const std::string& name) const;

private:
    NamespacedRegistry<TemplateDefinitionNode> m_templates;
    NamespacedRegistry<CustomDefinitionNode> m_customs;
};

} // namespace CHTL
