#include "Context.h"

namespace CHTL {

void CHTLContext::addStyleTemplate(const std::string& name, StyleTemplate&& style_template) {
    m_style_templates[name] = std::move(style_template);
}

const StyleTemplate* CHTLContext::getStyleTemplate(const std::string& name) const {
    auto it = m_style_templates.find(name);
    if (it != m_style_templates.end()) {
        return &it->second;
    }
    return nullptr;
}

bool CHTLContext::hasBeenImported(const std::string& path) const {
    for (const auto& imported_path : m_imported_paths) {
        if (imported_path == path) {
            return true;
        }
    }
    return false;
}

void CHTLContext::addImportedPath(const std::string& path) {
    m_imported_paths.push_back(path);
}

} // namespace CHTL
