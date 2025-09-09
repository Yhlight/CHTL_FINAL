#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "CHTLNode/BaseNode.h"

class CHTLContext {
public:
    using StyleProperties = std::map<std::string, std::string>;
    using ElementTemplate = std::vector<std::unique_ptr<BaseNode>>;
    using VarGroup = std::map<std::string, std::string>;

    // Style Templates
    void addStyleTemplate(const std::string& name, const StyleProperties& properties);
    const StyleProperties& getStyleTemplate(const std::string& name) const;
    bool hasStyleTemplate(const std::string& name) const;

    // Element Templates
    void addElementTemplate(const std::string& name, ElementTemplate elementTemplate);
    const ElementTemplate& getElementTemplate(const std::string& name) const;
    bool hasElementTemplate(const std::string& name) const;

    // Variable Templates
    void addVarTemplate(const std::string& groupName, const VarGroup& vars);
    const std::string& getVarTemplateValue(const std::string& groupName, const std::string& varName) const;
    bool hasVarTemplate(const std::string& groupName) const;

    // Global CSS
    void addGlobalCSS(const std::string& rule);
    const std::string& getGlobalCSS() const;

private:
    std::map<std::string, StyleProperties> styleTemplates;
    std::map<std::string, ElementTemplate> elementTemplates;
    std::map<std::string, VarGroup> varTemplates;
    std::string globalCSS;
};

#endif // CHTL_CONTEXT_H
