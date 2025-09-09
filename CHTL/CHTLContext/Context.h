#pragma once

#include "../CHTLNode/AST.h" // For CssPropertyNode
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// Represents a defined Style Template
struct StyleTemplate {
    std::string name;
    std::vector<CssPropertyNode> properties;
};

// CHTLContext holds the shared state of the compiler,
// such as symbol tables, template definitions, etc.
class CHTLContext {
public:
    // Style Template Management
    void addStyleTemplate(const std::string& name, StyleTemplate&& style_template);
    const StyleTemplate* getStyleTemplate(const std::string& name) const;

    // Import Management
    bool hasBeenImported(const std::string& path) const;
    void addImportedPath(const std::string& path);

private:
    std::map<std::string, StyleTemplate> m_style_templates;
    std::vector<std::string> m_imported_paths;
};

} // namespace CHTL
