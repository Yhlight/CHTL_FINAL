#pragma once

#include "../CHTLNode/TemplateNode.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace CHTL {

class Context {
public:
    void addTemplate(const std::string& name, std::unique_ptr<TemplateNode> templateNode) {
        m_templates[name] = std::move(templateNode);
    }

    TemplateNode* getTemplate(const std::string& name) const {
        auto it = m_templates.find(name);
        if (it != m_templates.end()) {
            return it->second.get();
        }
        return nullptr;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<TemplateNode>> m_templates;
};

} // namespace CHTL
