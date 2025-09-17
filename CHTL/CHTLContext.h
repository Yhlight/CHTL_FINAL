#pragma once

#include "CHTLNode/TemplateNode.h"
#include <string>
#include <map>
#include <memory>

namespace CHTL {

class CHTLContext {
public:
    void registerTemplate(const std::string& name, std::shared_ptr<TemplateNode> node) {
        m_templates[name] = node;
    }

    std::shared_ptr<TemplateNode> findTemplate(const std::string& name) {
        auto it = m_templates.find(name);
        if (it != m_templates.end()) {
            // lock() creates a shared_ptr from the weak_ptr.
            // If the object was deleted, it returns a null shared_ptr.
            return it->second.lock();
        }
        return nullptr;
    }

private:
    // Use weak_ptr to avoid circular references. The AST owns the nodes
    // via shared_ptr, and the context just holds a non-owning reference.
    std::map<std::string, std::weak_ptr<TemplateNode>> m_templates;
};

} // namespace CHTL
