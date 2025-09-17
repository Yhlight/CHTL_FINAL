#pragma once

#include "CHTLNode/TemplateNode.h"
#include <string>
#include <map>
#include <memory>

namespace CHTL {

class CHTLContext {
public:
    void registerTemplate(const std::string& name, TemplateNode* node) {
        m_templates[name] = node;
    }

    TemplateNode* findTemplate(const std::string& name) {
        auto it = m_templates.find(name);
        if (it != m_templates.end()) {
            return it->second;
        }
        return nullptr;
    }

private:
    // Stores a raw pointer to the TemplateNode.
    // The Node itself is owned by the main AST (unique_ptr).
    std::map<std::string, TemplateNode*> m_templates;
};

} // namespace CHTL
