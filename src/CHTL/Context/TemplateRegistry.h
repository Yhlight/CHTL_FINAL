#ifndef CHTL_TEMPLATE_REGISTRY_H
#define CHTL_TEMPLATE_REGISTRY_H

#include "../Node/BaseNode.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace CHTL {

class TemplateRegistry {
public:
    void registerDefinition(const std::string& name, std::shared_ptr<BaseNode> node);
    std::shared_ptr<BaseNode> lookupDefinition(const std::string& name);

    static TemplateRegistry& getInstance() {
        static TemplateRegistry instance;
        return instance;
    }

private:
    TemplateRegistry() = default;
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> m_templates;

public:
    // Prevent copying and assignment
    TemplateRegistry(const TemplateRegistry&) = delete;
    void operator=(const TemplateRegistry&) = delete;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_REGISTRY_H
