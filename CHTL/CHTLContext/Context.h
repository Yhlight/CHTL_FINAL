#pragma once

#include "../CHTLNode/TemplateNodes.h"
#include <string>
#include <memory>
#include <map>

// The Context class acts as a symbol table, storing definitions
// of templates, and later, custom components, namespaces, etc.
class Context {
public:
    // Style Templates
    void addStyleTemplate(const std::string& name, std::shared_ptr<StyleTemplateNode> node) {
        styleTemplates[name] = node;
    }
    std::shared_ptr<StyleTemplateNode> getStyleTemplate(const std::string& name) const {
        if (styleTemplates.count(name)) {
            return styleTemplates.at(name);
        }
        return nullptr;
    }

    // Element Templates
    void addElementTemplate(const std::string& name, std::shared_ptr<ElementTemplateNode> node) {
        elementTemplates[name] = node;
    }
    std::shared_ptr<ElementTemplateNode> getElementTemplate(const std::string& name) const {
        if (elementTemplates.count(name)) {
            return elementTemplates.at(name);
        }
        return nullptr;
    }

    // Variable Group Templates
    void addVarTemplate(const std::string& name, std::shared_ptr<VarTemplateNode> node) {
        varTemplates[name] = node;
    }
    std::shared_ptr<VarTemplateNode> getVarTemplate(const std::string& name) const {
        if (varTemplates.count(name)) {
            return varTemplates.at(name);
        }
        return nullptr;
    }


private:
    std::map<std::string, std::shared_ptr<StyleTemplateNode>> styleTemplates;
    std::map<std::string, std::shared_ptr<ElementTemplateNode>> elementTemplates;
    std::map<std::string, std::shared_ptr<VarTemplateNode>> varTemplates;
};
