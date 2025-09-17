#pragma once

#include "../CHTLNode/TemplateNodes.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace CHTL {

class CHTLEnvironment {
public:
    CHTLEnvironment() = default;

    std::shared_ptr<TemplateDefinitionNode> getTemplate(const std::string& name) const {
        auto it = m_templates.find(name);
        if (it != m_templates.end()) {
            return it->second;
        }
        return nullptr;
    }

    void setTemplate(const std::string& name, std::shared_ptr<TemplateDefinitionNode> node) {
        m_templates[name] = node;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<TemplateDefinitionNode>> m_templates;
};

} // namespace CHTL
