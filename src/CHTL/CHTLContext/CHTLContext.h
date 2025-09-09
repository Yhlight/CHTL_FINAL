#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stack>
#include "CHTLNode/BaseNode.h"

class CHTLContext {
public:
    using StyleProperties = std::map<std::string, std::string>;
    using ElementTemplate = std::vector<std::unique_ptr<BaseNode>>;
    using VarGroup = std::map<std::string, std::string>;

    CHTLContext();

    // Namespace Management
    void pushNamespace(const std::string& name);
    void popNamespace();
    std::string getCurrentNamespace() const;

    // Template Management (now namespace-aware)
    void addStyleTemplate(const std::string& name, const StyleProperties& properties);
    const StyleProperties& getStyleTemplate(const std::string& name, const std::string& ns = "") const;

    void addElementTemplate(const std::string& name, ElementTemplate elementTemplate);
    const ElementTemplate& getElementTemplate(const std::string& name, const std::string& ns = "") const;

    void addVarTemplate(const std::string& groupName, const VarGroup& vars);
    const std::string& getVarTemplateValue(const std::string& groupName, const std::string& varName, const std::string& ns = "") const;

    // Global CSS
    void addGlobalCSS(const std::string& rule);
    const std::string& getGlobalCSS() const;

private:
    std::map<std::string, std::map<std::string, StyleProperties>> namespacedStyleTemplates;
    std::map<std::string, std::map<std::string, ElementTemplate>> namespacedElementTemplates;
    std::map<std::string, std::map<std::string, VarGroup>> namespacedVarTemplates;

    std::stack<std::string> namespaceStack;
    std::string globalCSS;
};

#endif // CHTL_CONTEXT_H
