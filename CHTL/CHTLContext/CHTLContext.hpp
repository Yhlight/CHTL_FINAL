#pragma once

#include "../CHTLNode/Node.hpp"
#include <map>
#include <string>

namespace CHTL {

class CHTLContext {
public:
    void addTemplate(TemplateDefinitionNode* def) {
        if (!def) return;
        switch (def->type) {
            case TemplateType::ELEMENT:
                templateElements[def->name] = def;
                break;
            case TemplateType::STYLE:
                templateStyles[def->name] = def;
                break;
            case TemplateType::VAR:
                templateVars[def->name] = def;
                break;
        }
    }

    TemplateDefinitionNode* getElementTemplate(const std::string& name) {
        return templateElements.count(name) ? templateElements[name] : nullptr;
    }

    TemplateDefinitionNode* getStyleTemplate(const std::string& name) {
        return templateStyles.count(name) ? templateStyles[name] : nullptr;
    }

    TemplateDefinitionNode* getVarTemplate(const std::string& name) {
        return templateVars.count(name) ? templateVars[name] : nullptr;
    }

private:
    // Non-owning pointers. The AST Program node owns the actual nodes.
    std::map<std::string, TemplateDefinitionNode*> templateElements;
    std::map<std::string, TemplateDefinitionNode*> templateStyles;
    std::map<std::string, TemplateDefinitionNode*> templateVars;
};

} // namespace CHTL
