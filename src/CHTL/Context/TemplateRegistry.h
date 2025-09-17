#ifndef CHTL_TEMPLATE_REGISTRY_H
#define CHTL_TEMPLATE_REGISTRY_H

#include "../Node/TemplateNode.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace CHTL {

class TemplateRegistry {
public:
    void registerTemplate(const std::string& name, std::shared_ptr<TemplateNode> node);
    std::shared_ptr<TemplateNode> lookupTemplate(const std::string& name);

    static TemplateRegistry& getInstance() {
        static TemplateRegistry instance;
        return instance;
    }

private:
    TemplateRegistry() = default;
    std::unordered_map<std::string, std::shared_ptr<TemplateNode>> m_templates;

public:
    // Prevent copying and assignment
    TemplateRegistry(const TemplateRegistry&) = delete;
    void operator=(const TemplateRegistry&) = delete;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_REGISTRY_H
